#version 460 core

#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout : require

struct Vertex {
	float x, y, z;
	float nx, ny, nz;
	float u, v;
};

layout(buffer_reference, scalar) readonly buffer Vertices {
	Vertex in_Vertices[];
};

layout(location = 0) out vec2 uvs;

layout(push_constant) uniform PerFrameData {
		mat4 mvp;
		uint textureId;
		uint id;
		Vertices vtx;
}pc;

void main () {
		Vertex vtx = pc.vtx.in_Vertices[gl_VertexIndex];
		vec3 pos = vec3(vtx.x, vtx.y, vtx.z);
		gl_Position = pc.mvp * vec4(pos, 1.0f);
		uvs = vec2(vtx.u, vtx.v);
}
