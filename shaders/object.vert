#version 460 core
#include <objects.sp>

layout(location = 0) out flat uint instanceID;
layout(location = 1) out vec2 uv;
layout(location = 2) out vec3 normals;
layout(location = 3) out vec3 worldPos;

void main() {
		uint idx = gl_VertexIndex;
		instanceID = gl_InstanceIndex;
		Vertex vtx = pc.vtx.in_Vertices[idx];
		ObjectPerFrame obj = pc.objData.in_ObjectDatas[instanceID];

		vec4 pos = vec4(vtx.x, vtx.y, vtx.z, 1.0f);
		vec3 norm = vec3(vtx.nx, vtx.ny, vtx.nz);
		gl_Position = pc.pv * obj.model * pos;

		worldPos = vec3(obj.model * pos);
		normals = transpose(inverse(mat3(obj.model))) * norm;
		uv = vec2(vtx.u, vtx.v) * obj.tiling;
};
