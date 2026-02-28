const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float a) {
		float a2 = a * a;
		float NdotH = max(dot(N,H), 0.0f);
		float NdotH2 = NdotH * NdotH;

		float norm = a2;
		float denorm = (NdotH * (a2 - 1.0) + 1.0);
		denorm = PI * denorm * denorm;
		return norm / denorm;
}

float GeometrySchicleGGX(float NdotV, float k) {
		float nom = NdotV;
		float denom = NdotV * (1.0 - k) + k;
		return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float k) {
		float NdotV = max(dot(N,V), 0.0f);
		float NdotL = max(dot(N,L), 0.0f);
		float ggx1 = GeometrySchicleGGX(NdotV, k);
		float ggx2 = GeometrySchicleGGX(NdotL, k);
		return  ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 f0) {
		return f0 + (1.0 - f0)  * pow(clamp(1.0 - cosTheta, 0.0f, 1.0f), 5.0);
}

