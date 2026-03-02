#version 460 core
#include <model.sp>
#include <texture.sp>

layout(location = 0) in vec2 uv;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec3 worldPos;

layout(location = 0) out vec4 out_FragColor;

void main () {
		BufferPerFrame buff = pc.buffData.in_bufferData;

		vec3 albedo = textureBindless2D(pc.albedoTex, 0, uv).xyz;
		
		out_FragColor = vec4(albedo, 1.0f) * pc.baseColor;
}

