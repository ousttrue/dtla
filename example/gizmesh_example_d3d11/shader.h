#pragma once
#include <array>
#include <d3d11.h>
#include <string>
#include <wrl/client.h>

class Shader {
  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;
  ComPtr<ID3D11VertexShader> m_vs;
  ComPtr<ID3D11PixelShader> m_ps;
  ComPtr<ID3D11InputLayout> m_inputLayout;
  ComPtr<ID3D11Buffer> m_cb;
  ComPtr<ID3D11RasterizerState> m_rasterizerState;

public:
  bool initialize(ID3D11Device *device, const std::string &vs,
                  const std::string &vsEntryPoint, const std::string &ps,
                  const std::string &psEntryPoint);

  void setup(ID3D11DeviceContext *context, const std::array<float, 3> &eye,
             const std::array<float, 16> &viewProj,
             const std::array<float, 16> &model);
};
