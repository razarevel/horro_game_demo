#version 460 core

layout(location = 0) out uint colorId;


layout(push_constant) uniform PerFrameData {
		mat4 mvp;
		uint textureId;
		uint id;
}pc;

void main () {
		colorId = pc.id;
}
