#version 460 core
#include <objects.sp>
#include <texture.sp>

layout(location = 0) in flat uint idx;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normals;
layout(location = 3) in vec3 worldPos;

layout(location = 0) out vec4 out_FragColor;

void main () {
		ObjectPerFrame obj = pc.objData.in_ObjectDatas[idx];
		BufferPerFrame buff = pc.buffData.in_bufferData;

		vec3 albedo = obj.color.xyz;
		if(obj.albedoTex != 0)
				albedo = textureBindless2D(obj.albedoTex, 0, uv).xyz;
		
		out_FragColor = vec4(albedo, 1.0f);
}

