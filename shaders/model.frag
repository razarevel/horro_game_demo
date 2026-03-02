#version 460 core
#include <model.sp>
#include <texture.sp>
#include <pbr.frag>

layout(location = 0) in vec2 uv;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec3 worldPos;
layout(location = 3) in mat3 TBN;

layout(location = 0) out vec4 out_FragColor;
vec3 getNormals();

void main () {
		BufferPerFrame buff = pc.buffData.in_bufferData;

		vec3 lightPos = buff.lightPos.xyz;
		vec3 lightColor = buff.lightColor.xyz;
		vec3 cameraPos = buff.cameraPos.xyz;

		vec3 albedo = textureBindless2D(pc.albedoTex, 0, uv).xyz;
		vec3 arm = textureBindless2D(pc.albedoTex, 0, uv).xyz;
		float ao = arm.r;
		float metallic = arm.g;
		float roughness = arm.b;


		vec3 N = getNormals();
		vec3 V = normalize(cameraPos - worldPos);

		vec3 f0 = vec3(0.4);
		f0 = mix(f0, albedo, metallic);

		vec3 Lo = vec3(0.0f);

		// light
		vec3 L = normalize(lightPos - worldPos);
		vec3 H = normalize(V + L);
		float distance = length(lightPos - worldPos);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = lightColor * attenuation;

		// let him cook - torrance RBDF
		float NDF = DistributionGGX(N,H, roughness);
		float G = GeometrySmith(N,V,L,roughness);
		vec3 F = fresnelSchlick(clamp(dot(H,V), 0.0, 1.0), f0);

		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.0001;
		vec3 specular = numerator / denominator;

		// ks is equal to fresnal
		vec3 ks = F;
		vec3 kD = vec3(1.0f) - ks;
		kD *= 1.0 - metallic;

		float NdotL = max(dot(N, L), 0.0f);

		Lo += (kD * albedo / PI + specular) * radiance * NdotL;

		vec3 ambient = vec3(0.03f) * albedo * ao;

		vec3 color = ambient + Lo;

		color = color / (color + vec3(1.0f));

		
		out_FragColor = vec4(color, 1.0f) * pc.baseColor;
}


vec3 getNormals(){
		vec3 tangentNormal = textureBindless2D(pc.normTex, 0, uv).xyz * 2.0 - 1.0;
		return normalize(TBN * tangentNormal);
}
