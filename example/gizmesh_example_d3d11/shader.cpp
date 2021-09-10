#include "shader.h"
#include <d3dcompiler.h>
#include <iostream>

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

static ComPtr<ID3DBlob> LoadCompileShader(const SourceWithEntryPoint &src,
                                          const char *name,
                                          const D3D_SHADER_MACRO *define,
                                          const char *target) {
  UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;

  ComPtr<ID3DBlob> ret;
  ComPtr<ID3DBlob> err;
  if (FAILED(D3DCompile(src.source.data(), src.source.size(), name, define,
                        nullptr, src.entry_point.data(), target, flags, 0, &ret,
                        &err))) {
    auto error = (char *)err->GetBufferPointer();
    std::cerr << name << ": " << error << std::endl;
    // std::cerr << src.source << std::endl;
    return nullptr;
  }
  return ret;
}

bool Shader::initialize(ID3D11Device *device,
                        const std::optional<SourceWithEntryPoint> &vs,
                        const std::optional<SourceWithEntryPoint> &gs,
                        const std::optional<SourceWithEntryPoint> &ps) {

  if (vs) {
    auto vsBlob = LoadCompileShader(vs.value(), "vs", nullptr, "vs_4_0");
    if (!vsBlob) {
      return false;
    }
    if (FAILED(device->CreateVertexShader((DWORD *)vsBlob->GetBufferPointer(),
                                          vsBlob->GetBufferSize(), nullptr,
                                          &m_vs))) {
      return false;
    }

    D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    if (FAILED(device->CreateInputLayout(
            inputDesc, _countof(inputDesc), vsBlob->GetBufferPointer(),
            vsBlob->GetBufferSize(), &m_inputLayout))) {
      return false;
    }

    D3D11_BUFFER_DESC desc = {0};
    desc.ByteWidth = vs.value().cs_size;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    // desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    if (FAILED(device->CreateBuffer(&desc, nullptr, &m_cb))) {
      return false;
    }
  }

  if (gs) {
    auto blob = LoadCompileShader(gs.value(), "gs", nullptr, "gs_4_0");
    if (!blob) {
      return false;
    }
    if (FAILED(device->CreateGeometryShader((DWORD *)blob->GetBufferPointer(),
                                            blob->GetBufferSize(), nullptr,
                                            &m_gs))) {
      return false;
    }
  }

  if (ps) {
    auto psBlob = LoadCompileShader(ps.value(), "ps", nullptr, "ps_4_0");
    if (!psBlob) {
      return false;
    }
    if (FAILED(device->CreatePixelShader((DWORD *)psBlob->GetBufferPointer(),
                                         psBlob->GetBufferSize(), nullptr,
                                         &m_ps))) {
      return false;
    }
  }

  {
    D3D11_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    rasterizerDesc.FrontCounterClockwise = true;

    if (FAILED(device->CreateRasterizerState(&rasterizerDesc,
                                             &m_rasterizerState))) {
      return false;
    }
  }

  return true;
}

void Shader::setup(ID3D11DeviceContext *context, const void *p, uint32_t size) {
  if (!m_vs) {
    return;
  }

  context->UpdateSubresource(m_cb.Get(), 0, nullptr, p, 0, 0);

  ID3D11Buffer *cb_list[] = {m_cb.Get()};
  context->IASetInputLayout(m_inputLayout.Get());

  context->VSSetShader(m_vs.Get(), nullptr, 0);
  context->VSSetConstantBuffers(0, _countof(cb_list), cb_list);

  if (m_gs) {
    context->GSSetShader(m_gs.Get(), nullptr, 0);
  } else {
    context->GSSetShader(0, nullptr, 0);
  }

  context->PSSetShader(m_ps.Get(), nullptr, 0);
  context->PSSetConstantBuffers(0, _countof(cb_list), cb_list);

  context->OMSetBlendState(nullptr, nullptr, 0xffffffff);
  context->OMSetDepthStencilState(nullptr, 0);
  context->RSSetState(m_rasterizerState.Get());
}
