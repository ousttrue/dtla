#include "OrbitCamera.h"
#include "falg.h"
#define _USE_MATH_DEFINES
#include <cmath>

void OrbitCamera::CalcView(int w, int h, int x, int y) {
  // view transform
  auto origin = falg::Transform{gaze, {0, 0, 0, 1}};
  auto q_yaw = falg::QuaternionAxisAngle({0, 1, 0}, yawRadians);
  auto q_pitch = falg::QuaternionAxisAngle({1, 0, 0}, pitchRadians);
  auto transform =
      origin * falg::Transform{shift, falg::QuaternionMul(q_yaw, q_pitch)};
  state.view = transform.RowMatrix();

  // inverse view transform
  auto inv = transform.Inverse();
  {
    state.rotation = inv.rotation;
    state.position = inv.translation;
  }

  // ray for mouse cursor
  auto t = std::tan(state.fovYRadians / 2);
  const float xx = 2 * (float)x / w - 1;
  const float yy = 1 - 2 * (float)y / h;
  auto dir = falg::float3{
      t * aspectRatio * xx,
      t * yy,
      -1,
  };
  state.ray_direction = inv.ApplyDirection(dir);
  state.ray_origin = state.position;
}

void OrbitCamera::CalcPerspective() {
  switch (perspectiveType) {
  case PerspectiveTypes::OpenGL:
    falg::PerspectiveRHGL(state.projection.data(), state.fovYRadians,
                          aspectRatio, zNear, zFar);
    break;

  case PerspectiveTypes::D3D:
    falg::PerspectiveRHDX(state.projection.data(), state.fovYRadians,
                          aspectRatio, zNear, zFar);
    break;
  }
}

void OrbitCamera::SetViewport(int x, int y, int w, int h) {
  if (w == state.viewportWidth && h == state.viewportHeight) {
    return;
  }

  if (h == 0 || w == 0) {
    aspectRatio = 1.0f;
  } else {
    aspectRatio = w / (float)h;
  }
  state.viewportX = x;
  state.viewportY = y;
  state.viewportWidth = w;
  state.viewportHeight = h;
  CalcPerspective();

  return;
}

void OrbitCamera::Update(const screenstate::ScreenState &window) {
  SetViewport(0, 0, window.Width, window.Height);

  if (prevMouseX != -1 && prevMouseY != -1) {
    auto deltaX = window.MouseX - prevMouseX;
    auto deltaY = window.MouseY - prevMouseY;

    if (window.Has(screenstate::MouseButtonFlags::RightDown)) {
      const auto FACTOR = 1.0f / 180.0f * 1.7f;
      yawRadians += deltaX * FACTOR;
      pitchRadians += deltaY * FACTOR;
    }
    if (window.Has(screenstate::MouseButtonFlags::MiddleDown)) {
      shift[0] -= deltaX / (float)state.viewportHeight * shift[2];
      shift[1] += deltaY / (float)state.viewportHeight * shift[2];
    }
    if (window.Has(screenstate::MouseButtonFlags::WheelPlus)) {
      shift[2] *= 0.9f;
    } else if (window.Has(screenstate::MouseButtonFlags::WheelMinus)) {
      shift[2] *= 1.1f;
    }
  }
  prevMouseX = window.MouseX;
  prevMouseY = window.MouseY;
  CalcView(window.Width, window.Height, window.MouseX, window.MouseY);
}
