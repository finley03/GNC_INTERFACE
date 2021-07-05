// Grid fragment shader

#version 460 core

in vec3 FragPos;

uniform vec3 position;

out vec4 FragColor;

float logab(float base, float b) {
	return log2(b) / log2(base);
}

void main() {
	float distance = length(FragPos - position);
	float threshold = 0.005 * sqrt(distance);//logab(15, distance);
	float thresholdSmall = threshold * 3;
	vec3 color = vec3(0.5, 0.5, 0.5);

	float Xmul = abs(FragPos.x - round(FragPos.x)) / threshold;
	float Zmul = abs(FragPos.z - round(FragPos.z)) / threshold;
	float XmulSmall = abs(FragPos.x * 10 - round(FragPos.x * 10))  / thresholdSmall;
	float ZmulSmall = abs(FragPos.z * 10 - round(FragPos.z * 10))  / thresholdSmall;

	float alphaMul = min(1.0, 1.0 / (distance / 7));
	float alphaMulSmall = min(1.0, 1.0 / (distance / 1.0));

	float Xalpha = exp(-2.7 * Xmul * Xmul * Xmul);
	float Zalpha = exp(-2.7 * Zmul * Zmul * Zmul);
	float XalphaSmall = exp(-2.7 * XmulSmall * XmulSmall * XmulSmall);
	float ZalphaSmall = exp(-2.7 * ZmulSmall * ZmulSmall * ZmulSmall);

	//float Xalpha = 1.0 / pow(Xmul, 3);
	//float Zalpha = 1.0 / pow(Zmul, 3);


	FragColor = vec4(color, max(max(Xalpha, Zalpha) * alphaMul, max(XalphaSmall, ZalphaSmall) * alphaMulSmall));

}
