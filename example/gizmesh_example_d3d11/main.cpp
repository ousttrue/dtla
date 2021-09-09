﻿#include "OrbitCamera.h"
#include "Win32Window.h"
#include "renderer.h"
#include "teapot.h"
#include <d3d11.h>
#include <gizmesh.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <vector>
#include <wrl/client.h>

struct vertex {
  std::array<float, 3> position;
  std::array<float, 3> normal;
  std::array<float, 4> color;
};
static_assert(sizeof(vertex) == 40, "vertex size");

enum class transform_mode { translate, rotate, scale };

#include <iostream>
#include <ranges>
#include <vector>

//////////////////////////
//   Main Application   //
//////////////////////////
int main(int argc, char *argv[]) {
  screenstate::Win32Window win(L"tinygizmo_example_class");
  auto hwnd = win.Create(L"tinygizmo_example_d3d11", 1280, 800);
  if (!hwnd) {
    return 1;
  }
  win.Show();

  // camera
  OrbitCamera camera(PerspectiveTypes::D3D);
  transform_mode mode = transform_mode::scale;
  bool is_local = true;

  //
  // scene
  //
  Renderer renderer;
  auto device = (ID3D11Device *)renderer.initialize(hwnd);
  if (!device) {
    return 3;
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
  device->GetImmediateContext(&context);
  ImGui_ImplDX11_Init(device, context.Get());

  // create teapot
  auto teapot_mesh = renderer.createMesh();
  std::vector<vertex> vertices;
  for (int i = 0; i < 4974; i += 6) {
    vertex v{0};
    v.position = {teapot_vertices[i + 0], teapot_vertices[i + 1],
                  teapot_vertices[i + 2]};
    v.normal = {teapot_vertices[i + 3], teapot_vertices[i + 4],
                teapot_vertices[i + 5]};
    vertices.push_back(v);
  }
  teapot_mesh->uploadMesh(
      device, vertices.data(),
      static_cast<uint32_t>(vertices.size() * sizeof(vertex)), sizeof(vertex),
      teapot_triangles, sizeof(teapot_triangles), sizeof(teapot_triangles[0]),
      false);

  // teapot a
  falg::TRS teapot_a;
  teapot_a.translation = {-2, 0, 0};

  // teapot b
  falg::TRS teapot_b;
  teapot_b.translation = {+2, 0, 0};

  // gizmo
  gizmesh::GizmoSystem system;
  auto gizmo_mesh = renderer.createMesh();

  //
  // main loop
  //
  screenstate::ScreenState state;
  std::bitset<128> lastState{};
  for (int i = 0;; ++i) {
    if (!win.TryGetInput(&state)) {
      exit(1);
    }

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    // ImGui_ImplWin32_NewFrame();
    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize = ImVec2(state.Width, state.Height);

    ImGui::NewFrame();
    ImGui::ShowDemoWindow();
    ImGui::Render();

    // update camera
    camera.Update(state);

    // gizmo new frame
    system.begin(camera.state.position, camera.state.rotation,
                 camera.state.ray_origin, camera.state.ray_direction,
                 state.MouseLeftDown());

    if (state.KeyCode['R']) {
      mode = transform_mode::rotate;
    }
    if (state.KeyCode['T']) {
      mode = transform_mode::translate;
    }
    if (state.KeyCode['S']) {
      mode = transform_mode::scale;
    }
    if (!lastState['Z'] && state.KeyCode['Z']) {
      is_local = !is_local;
    }
    lastState = state.KeyCode;

    auto viewProjection = camera.state.view * camera.state.projection;

    //
    // draw
    //
    auto context = renderer.beginFrame(state.Width, state.Height);
    teapot_mesh->draw(context, teapot_a.RowMatrix().data(),
                      viewProjection.data(), camera.state.position.data());
    teapot_mesh->draw(context, teapot_b.RowMatrix().data(),
                      viewProjection.data(), camera.state.position.data());

    {
      //
      // manipulate and update gizmo
      //
      switch (mode) {
      case transform_mode::translate:
        gizmesh::handle::translation(
            system, gizmesh::hash_fnv1a("first-example-gizmo"), is_local,
            nullptr, teapot_a.translation, teapot_a.rotation);
        gizmesh::handle::translation(
            system, gizmesh::hash_fnv1a("second-example-gizmo"), is_local,
            nullptr, teapot_b.translation, teapot_b.rotation);
        break;

      case transform_mode::rotate:
        gizmesh::handle::rotation(
            system, gizmesh::hash_fnv1a("first-example-gizmo"), is_local,
            nullptr, teapot_a.translation, teapot_a.rotation);
        gizmesh::handle::rotation(
            system, gizmesh::hash_fnv1a("second-example-gizmo"), is_local,
            nullptr, teapot_b.translation, teapot_b.rotation);
        break;

      case transform_mode::scale:
        gizmesh::handle::scale(
            system, gizmesh::hash_fnv1a("first-example-gizmo"), is_local,
            teapot_a.translation, teapot_a.rotation, teapot_a.scale);
        gizmesh::handle::scale(
            system, gizmesh::hash_fnv1a("second-example-gizmo"), is_local,
            teapot_b.translation, teapot_b.rotation, teapot_b.scale);
        break;
      }

      auto buffer = system.end();
      gizmo_mesh->uploadMesh(device, buffer.pVertices, buffer.verticesBytes,
                             buffer.vertexStride, buffer.pIndices,
                             buffer.indicesBytes, buffer.indexStride, true);
    }

    //
    // gizmo after xform user draw
    //
    renderer.clearDepth();
    static const float identity4x4[] = {
        1, 0, 0, 0, //
        0, 1, 0, 0, //
        0, 0, 1, 0, //
        0, 0, 0, 1, //
    };
    gizmo_mesh->draw(context, identity4x4, viewProjection.data(),
                     camera.state.position.data());

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    //
    // present
    //
    renderer.endFrame();
  }

  ImGui_ImplDX11_Shutdown();
  return EXIT_SUCCESS;
}
