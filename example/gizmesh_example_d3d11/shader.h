#pragma once
#include <array>
#include <d3d11.h>
#include <string>
#include <wrl/client.h>

struct SourceWithEntryPoint {
  std::string_view source;
  std::string_view entry_point;
};

class Shader {
  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;
  ComPtr<ID3D11VertexShader> m_vs;
  ComPtr<ID3D11PixelShader> m_ps;

  ComPtr<ID3D11InputLayout> m_inputLayout;
  ComPtr<ID3D11Buffer> m_cb;
  ComPtr<ID3D11RasterizerState> m_rasterizerState;

public:
  bool initialize(ID3D11Device *device, const SourceWithEntryPoint &vs,
                  const SourceWithEntryPoint &ps);

  void setup(ID3D11DeviceContext *context, const std::array<float, 3> &eye,
             const std::array<float, 16> &viewProj,
             const std::array<float, 16> &model);
};
