#version 460 core
#include <shape.sp>
#include <pbr.frag>

#extension GL_EXT_nonuniform_qualifier : require

layout(set = 0, binding = 0) uniform texture2D kTextures2D[];
layout(set = 0, binding = 1) uniform sampler kSamplers[];

layout(location = 0) out vec4 out_FragColor;

layout(location = 0) in vec2 uv;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec3 worldPos;


vec4 textureBindless2D(uint textureid, uint samplerid, vec2 uv);
vec3 getNormalFromMap();

BufferPerFrame perFrame = pc.data.in_bufferData;

void main () {
		vec3 lightPos = perFrame.lightPos.xyz;
		vec3 lightColor = perFrame.lightColor.xyz;
		vec3 cameraPos = perFrame.cameraPos.xyz;

		vec3 albedo;
		if(perFrame.albedoTex != 0)
				albedo = textureBindless2D(perFrame.albedoTex, 0, uv).xyz;

		float ao;
		float roughness;
		float metallic;

		if(perFrame.armTex != 0){
				vec3 arm = textureBindless2D(perFrame.armTex, 0, uv).xyz; 
				ao = arm.r;
				roughness = arm.g;
				metallic = arm.b;
		}

		vec3 N = getNormalFromMap();
		vec3 V = normalize(cameraPos- worldPos);

		vec3 f0 = vec3(0.4);
		f0 = mix(f0, albedo, metallic);

		vec3 Lo = vec3(0.0f);

		// light
		vec3 L = normalize(lightPos - worldPos);
		vec3 H = normalize(V + L);
		float distance = length(lightPos - worldPos);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = lightColor * attenuation;

		// let him Cook - torrance BRDF
		float NDF = DistributionGGX(N,H, roughness);
		float G = GeometrySmith(N,V,L, roughness);
		vec3 F = fresnelSchlick(clamp(dot(H,V), 0.0, 1.0), f0);

		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.0001;
		vec3 specular = numerator / denominator;

		// ks is equal to fresnal
		vec3 ks = F;
		vec3 kD = vec3(1.0f) - ks;
		kD*= 1.0 - metallic;

		float NdotL = max(dot(N, L), 0.0);

		Lo += (kD * albedo / PI + specular) * radiance * NdotL;

		vec3 ambient = vec3(0.03) * albedo * ao;

		vec3 color = ambient + Lo;

		color = color / (color + vec3(1.0f));

		out_FragColor = vec4(color, 1.0);
}

vec3 getNormalFromMap(){
		vec3 tangentNormal = textureBindless2D(perFrame.normTex, 0, uv).xyz * 2.0 - 1.0;

		vec3 Q1  = dFdx(worldPos);
		vec3 Q2  = dFdy(worldPos);
		vec2 st1 = dFdx(uv);
		vec2 st2 = dFdy(uv);

		vec3 N = normalize(normals);
		vec3 T = normalize(Q1*st2.t - Q2*st1.t);
		vec3 B = -normalize(cross(N, T));
		mat3 TBN = mat3(T,B,N);

		return normalize(TBN * tangentNormal);
}

vec4 textureBindless2D(uint textureid, uint samplerid, vec2 uv) {
    return texture(nonuniformEXT(sampler2D(kTextures2D[textureid], kSamplers[samplerid])), uv);
}
