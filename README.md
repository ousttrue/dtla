# gizmesh

gizmo library based https://github.com/ddiakopoulos/tinygizmo .
Thanks for tinygizmo.
Modification of ImGuizmo or Im3D was difficult.

* c++20
* d3d11

fpalg library is "FloatingPointAlgebra".

* use std::array as float2, float3, float4, float16.

## TODO

* [ ] restore snap

## gizmo: system

```c++
gizmesh::GizmoSystem system;
```

## gizmo: on new frame

```c++
// gizmo new frame
system.new_frame(
    camera.state.position, 
    camera.state.rotation,
    camera.state.ray_origin,
    camera.state.ray_direction,
    state.MouseLeftDown()
    );
```

Gizmo mode status, window handling, keyboard and mouse event handling is application's responsibilities.
gizmo receive only 5 params.

## gizmo: manipulate TRS transform

```c++
namespace gizmesh::handle
{

bool translation(const GizmoSystem &system, uint32_t id, fpalg::TRS &t, bool is_local);
bool rotation(const GizmoSystem &system, uint32_t id, fpalg::TRS &t, bool is_local);
bool scale(const GizmoSystem &system, uint32_t id, fpalg::TRS &t, bool is_uniform);

} // namespace gizmesh::handle

fpalg::TRS teapot_a;
teapot_a.position = {-2, 0, 0};
gizmesh::handle::translation(system, gizmesh::hash_fnv1a("first-example-gizmo"), teapot_a, is_local);
```

teapot_a transform may updated.

## gizmo: render to vertex buffer

```c++
// vertex format
struct vertex
{
    std::array<float, 3> position;
    std::array<float, 3> normal;
    std::array<float, 4> color;
};
static_assert(sizeof(vertex) == 40, "vertex size");

void *pVertices;
uint32_t verticesBytes;
uint32_t vertexStride;
void *pIndices;
uint32_t indicesBytes;
uint32_t indexStride;
system.render(
    &pVertices, &verticesBytes, &vertexStride,
    &pIndices, &indicesBytes, &indexStride);
```

## example

* example/gizmesh_example_gl3 (base from tinygizmo)
* example/gizmesh_example_d3d11
