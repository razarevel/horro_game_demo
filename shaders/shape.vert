#version 460 core
#include <shape.sp>

layout(location = 0) out vec2 uv;
layout(location = 1) out vec3 normals;
layout(location = 2) out vec3 worldPos;


void main () {
  Vertex vtx = pc.vtx.in_Vertices[gl_VertexIndex];
	vec4 pos = vec4(vtx.x, vtx.y, vtx.z, 1.0);
	vec3 norm = vec3(vtx.nx, vtx.ny, vtx.nz); 
	gl_Position = pc.proj * pc.view * pc.model * pos;

	worldPos = vec3(pc.model * pos);
	normals = transpose(inverse(mat3(pc.model))) * norm;
	uv = vec2(vtx.u, vtx.v) * pc.tiling;
}
