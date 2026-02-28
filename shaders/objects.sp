#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout: require

struct Vertex {
		float x, y, z;
		float nx, ny, nz;
		float u, v;
};

struct ObjectPerFrame{
		mat4 model;
		vec4 color;
		float tiling;
		float ao;
		float metallic;
		float roughenss;
		uint albedoTex;
		uint normTex;
		uint armTex;
		uint padding;
};

struct BufferPerFrame{
		vec4 lightPos;
		vec4 lightColor;
		vec4 cameraPos;
};

layout(buffer_reference, scalar) readonly buffer Vertices{
		Vertex in_Vertices[];
};

layout(buffer_reference, scalar) readonly buffer ObjectPerFrameData{
		ObjectPerFrame in_ObjectDatas[];
};

layout(buffer_reference, scalar) readonly buffer BufferPerFrameData{
		BufferPerFrame in_bufferData;
};

layout(push_constant) uniform PushConstants {
		mat4 pv;
		Vertices vtx;
		ObjectPerFrameData objData;
		BufferPerFrameData buffData;
}pc;
