#version 460 core
#include <texture.sp>

#extension GL_EXT_nonuniform_qualifier : require
layout(set = 0, binding = 0) uniform texture2D kTextures2D[];
layout(set = 0, binding = 1) uniform sampler kSamplers[];

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 out_FragColor;

layout(push_constant) uniform PushConstant{
		mat4 mvp;
		uint texId;
};

void main () {
		out_FragColor = textureBindless2D(texId, 0, uv);
}
