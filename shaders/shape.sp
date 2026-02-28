#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout: require

struct Vertex {
		float x, y, z;
		float nx, ny, nz;
		float u, v;
};

layout(buffer_reference, scalar) readonly buffer Vertices{
		Vertex in_Vertices[];
};

struct BufferPerFrame{
		vec4 lightPos;
		vec4 lightColor;
		vec4 cameraPos;
		uint albedoTex;
		uint normTex;
		uint armTex;
};

layout(buffer_reference, scalar) readonly buffer BufferPerFrameData{
		BufferPerFrame in_bufferData;
};

layout(push_constant) uniform PushConstants {
	mat4 proj;
	mat4 view;
	mat4 model;
	float tiling;
	Vertices vtx;
	BufferPerFrameData data;
}pc;
