#version 460 core
#include <model.sp>

layout(location = 0) out vec2 uv;
layout(location = 1) out vec3 normals;
layout(location = 2) out vec3 worldPos;

void main() {
		uint idx = gl_VertexIndex;
		Vertex vtx = pc.vtx.in_Vertices[idx];

		vec4 pos = vec4(vtx.x, vtx.y, vtx.z, 1.0f);
		vec3 norm = vec3(vtx.nx, vtx.ny, vtx.nz);
		gl_Position = pc.pv * pc.model * pos;

		worldPos = vec3(pc.model * pos);
		normals = transpose(inverse(mat3(pc.model))) * norm;
		uv = vec2(vtx.u, vtx.v);
}
