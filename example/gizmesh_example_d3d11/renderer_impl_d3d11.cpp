#include "dx11_context.h"
#include "renderer.h"
#include "teapot.h"
#include <DirectXMath.h>
#include <array>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <wrl/client.h>

template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

struct Vertex {
  DirectX::XMFLOAT3 position;
  DirectX::XMFLOAT3 normal;
  DirectX::XMFLOAT4 color;
};

constexpr const char gizmo_shader[] = R"(
  struct VS_INPUT
	{
		float3 position : POSITION;
    float3 normal   : NORMAL;
		float4 color    : COLOR0;
	};
  struct VS_OUTPUT
  {
    linear float4 position: SV_POSITION;
    linear float3 normal  : NORMAL;
    linear float4 color   : COLOR0;
    linear float3 world   : POSITION;
  };    
	cbuffer cbContextData : register(b0)
	{
		float4x4 uModel;
		float4x4 uViewProj;
    float3 uEye;
	};
    
  VS_OUTPUT vsMain(VS_INPUT _in) 
  {
    VS_OUTPUT ret;
    ret.world = mul(uModel, float4(_in.position, 1)).xyz;
    ret.position = mul(uViewProj, float4(ret.world, 1));
    ret.normal = _in.normal;
    ret.color = _in.color;
    return ret;
  }

	float4 psMain(VS_OUTPUT _in): SV_Target
  {
    float3 light = float3(1, 1, 1) * max(dot(_in.normal, normalize(uEye - _in.world)), 0.50) + 0.25;
    return _in.color * float4(light, 1);
  }
)";

constexpr const char lit_shader[] = R"(
  struct VS_INPUT
	{
		float3 position : POSITION;
    float3 normal   : NORMAL;
		float4 color    : COLOR0;
	};
  struct VS_OUTPUT
  {
    linear float4 position: SV_POSITION;
    linear float3 normal  : NORMAL;
    linear float4 color   : COLOR0;
    linear float3 world   : POSITION;
  };    
	cbuffer cbContextData : register(b0)
	{
		float4x4 uModel;
		float4x4 uViewProj;
    float3 uEye;
	};

  VS_OUTPUT vsMain(VS_INPUT _in) 
  {
    VS_OUTPUT ret;
    ret.world = mul(uModel, float4(_in.position, 1)).xyz;
    ret.position = mul(uViewProj, float4(ret.world, 1));
    ret.normal = _in.normal;
    ret.color = _in.color;
    return ret;
  }

	float4 psMain(VS_OUTPUT _in): SV_Target
  {
    float3 light = float3(0.5, 0.3, 0.3) * max(dot(_in.normal, normalize(uEye - _in.world)), 0.50) + 0.25;
    // return _in.color * float4(light, 1);
    return float4(light, 1);
  }
)";

#include "shader.h"

struct ConstantBuffer {
  std::array<float, 16> model;
  std::array<float, 16> viewProjection;
  std::array<float, 3> eye;
  float padding;
};
static_assert(sizeof(ConstantBuffer) == sizeof(float) * (16 * 2 + 4));

class ModelImpl {
  std::unique_ptr<Shader> m_shader;

  ComPtr<ID3D11Buffer> m_vb;

  ComPtr<ID3D11Buffer> m_ib;
  DXGI_FORMAT m_indexFormat = DXGI_FORMAT_R32_UINT;
  int m_indexCount = 0;

public:
  void upload_dynamic_mesh(ID3D11Device *device, const uint8_t *pVertices,
                           uint32_t verticesBytes, uint32_t vertexStride,
                           const uint8_t *pIndices, uint32_t indicesBytes,
                           uint32_t indexStride) {
    ComPtr<ID3D11DeviceContext> context;
    device->GetImmediateContext(&context);

    if (!m_shader) {
      m_shader.reset(new Shader);
      if (!m_shader->initialize(
              device, SourceWithEntryPoint{gizmo_shader, "vsMain", sizeof(ConstantBuffer)},
              std::nullopt, SourceWithEntryPoint{gizmo_shader, "psMain"})) {
        return;
      }
    }
    if (!m_vb) {
      D3D11_BUFFER_DESC desc{0};
      desc.ByteWidth = sizeof(Vertex) * 65535;
      desc.Usage = D3D11_USAGE_DYNAMIC;
      desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

      if (FAILED(device->CreateBuffer(&desc, nullptr, &m_vb))) {
        return;
      }
    }
    {
      D3D11_MAPPED_SUBRESOURCE mapped;
      context->Map(m_vb.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
      memcpy(mapped.pData, pVertices, verticesBytes);
      context->Unmap(m_vb.Get(), 0);
    }

    m_indexCount = indicesBytes / indexStride;
    if (!m_ib) {
      switch (indexStride) {
      case 4:
        m_indexFormat = DXGI_FORMAT_R32_UINT;
        break;
      case 2:
        m_indexFormat = DXGI_FORMAT_R16_UINT;
        break;
      default:
        throw;
      }
      D3D11_BUFFER_DESC desc = {0};
      desc.ByteWidth = indexStride * 65535;
      desc.Usage = D3D11_USAGE_DYNAMIC;
      desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
      desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

      if (FAILED(device->CreateBuffer(&desc, nullptr, &m_ib))) {
        return;
      }
    }
    {
      D3D11_MAPPED_SUBRESOURCE mapped;
      context->Map(m_ib.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
      memcpy(mapped.pData, pIndices, indicesBytes);
      context->Unmap(m_ib.Get(), 0);
    }
  }

  void upload_static_mesh(ID3D11Device *device, const uint8_t *pVertices,
                          uint32_t verticesBytes, uint32_t vertexStride,
                          const uint8_t *pIndices, uint32_t indicesBytes,
                          uint32_t indexStride) {
    if (!m_shader) {
      m_shader.reset(new Shader);
      if (!m_shader->initialize(
              device, SourceWithEntryPoint{lit_shader, "vsMain", sizeof(ConstantBuffer)}, std::nullopt,
              SourceWithEntryPoint{lit_shader, "psMain"})) {
        return;
      }
    }
    if (!m_vb) {
      D3D11_BUFFER_DESC desc{0};
      desc.ByteWidth = verticesBytes;
      desc.Usage = D3D11_USAGE_IMMUTABLE;
      desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

      D3D11_SUBRESOURCE_DATA subRes = {0};
      subRes.pSysMem = pVertices;

      if (FAILED(device->CreateBuffer(&desc, &subRes, &m_vb))) {
        return;
      }
    }
    if (!m_ib) {
      m_indexCount = indicesBytes / indexStride;
      switch (indexStride) {
      case 4:
        m_indexFormat = DXGI_FORMAT_R32_UINT;
        break;
      case 2:
        m_indexFormat = DXGI_FORMAT_R16_UINT;
        break;
      default:
        throw;
      }
      D3D11_BUFFER_DESC desc = {0};
      desc.ByteWidth = indicesBytes;
      desc.Usage = D3D11_USAGE_IMMUTABLE;
      desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

      D3D11_SUBRESOURCE_DATA subRes = {0};
      subRes.pSysMem = pIndices;

      if (FAILED(device->CreateBuffer(&desc, &subRes, &m_ib))) {
        return;
      }
    }
  }

  void draw(ID3D11DeviceContext *context, const std::array<float, 3> &eye,
            const std::array<float, 16> &viewProj,
            const std::array<float, 16> &model) {
    if (!m_shader) {
      return;
    }
    if (!m_vb || !m_ib) {
      return;
    }

    ConstantBuffer data{0};
    data.model = model;
    data.viewProjection = viewProj;
    data.eye = eye;
    m_shader->setup(context, &data, (uint32_t)sizeof(data));

    ID3D11Buffer *vbs[]{m_vb.Get()};
    UINT strides[]{sizeof(Vertex)};
    UINT offsets[]{0};
    context->IASetVertexBuffers(0, 1, vbs, strides, offsets);
    context->IASetIndexBuffer(m_ib.Get(), m_indexFormat, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->DrawIndexed(m_indexCount, 0, 0);
  }
};

Model::Model(ModelImpl *impl) : m_impl(impl) {}

Model::~Model() { delete m_impl; }

void Model::uploadMesh(void *device, const void *vertices,
                       uint32_t verticesSize, uint32_t vertexStride,
                       const void *indices, uint32_t indicesSize,
                       uint32_t indexSize, bool is_dynamic) {
  if (is_dynamic) {
    m_impl->upload_dynamic_mesh(
        (ID3D11Device *)device, (const uint8_t *)vertices, verticesSize,
        vertexStride, (const uint8_t *)indices, indicesSize, indexSize);
  } else {
    m_impl->upload_static_mesh(
        (ID3D11Device *)device, (const uint8_t *)vertices, verticesSize,
        vertexStride, (const uint8_t *)indices, indicesSize, indexSize);
  }
}

void Model::draw(void *context, const float *model, const float *vp,
                 const float *eye) {
  m_impl->draw((ID3D11DeviceContext *)context, *(std::array<float, 3> *)eye,
               *(std::array<float, 16> *)vp, *(std::array<float, 16> *)model);
}

/////////////////////////////////////////////
class RendererImpl {
  DX11Context m_d3d11;
  int m_width = 0;
  int m_height = 0;

public:
  void *initialize(void *hwnd) { return m_d3d11.Create(hwnd); }

  void *beginFrame(int width, int height) {
    return m_d3d11.NewFrame(width, height);
  }

  void clearDepth() { m_d3d11.ClearDepth(); }

  void endFrame() { m_d3d11.Present(); }
};

Renderer::Renderer() : m_impl(new RendererImpl) {}

Renderer::~Renderer() { delete m_impl; }

void *Renderer::initialize(void *hwnd) { return m_impl->initialize(hwnd); }

std::shared_ptr<Model> Renderer::createMesh() {
  auto modelImpl = new ModelImpl();
  return std::make_shared<Model>(modelImpl);
}

void *Renderer::beginFrame(int width, int height) {
  return m_impl->beginFrame(width, height);
}

void Renderer::endFrame() { m_impl->endFrame(); }

void Renderer::clearDepth() { m_impl->clearDepth(); }
