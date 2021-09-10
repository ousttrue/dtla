#pragma once
#include <array>
#include <d3d11.h>
#include <optional>
#include <string>
#include <wrl/client.h>

struct SourceWithEntryPoint {
  std::string_view source;
  std::string_view entry_point;
  uint32_t cs_size;
};

class Shader {
  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;
  ComPtr<ID3D11VertexShader> m_vs;
  ComPtr<ID3D11GeometryShader> m_gs;
  ComPtr<ID3D11PixelShader> m_ps;

  ComPtr<ID3D11InputLayout> m_inputLayout;
  ComPtr<ID3D11Buffer> m_cb;
  ComPtr<ID3D11RasterizerState> m_rasterizerState;

public:
  bool initialize(ID3D11Device *device,
                  const std::optional<SourceWithEntryPoint> &vs,
                  const std::optional<SourceWithEntryPoint> &gs,
                  const std::optional<SourceWithEntryPoint> &ps);

  void setup(ID3D11DeviceContext *context, const void *p, uint32_t size);
};
