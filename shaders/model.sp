#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout: require

struct Vertex {
		float x, y, z;
		float nx, ny, nz;
		float u, v;
		float tx, ty, tz;
		float bx, by, bz;
};

struct BufferPerFrame{
		vec4 lightPos;
		vec4 lightColor;
		vec4 cameraPos;
};

layout(buffer_reference, scalar) readonly buffer Vertices{
		Vertex in_Vertices[];
};

layout(buffer_reference, scalar) readonly buffer BufferPerFrameData{
		BufferPerFrame in_bufferData;
};

layout(push_constant) uniform PushConstants {
		mat4 pv;
		mat4 model;
		vec4 baseColor;
		uint albedoTex;
		uint normTex;
		uint armTex;
		Vertices vtx;
		BufferPerFrameData buffData;
}pc;
