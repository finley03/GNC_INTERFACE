// Grid fragment shader

#version 460 core

// there are 4 scales of graduations being rendered
#define LEVELS 4

// fragment position in world coordinates
in vec3 FragPos;

// camera positon and target
uniform vec3 position;
uniform vec3 target;

// grid color
uniform vec3 color;

// output fragment color
out vec4 FragColor;

// function to create arbitrary log functions
float logab(float base, float b) {
	return log(b) / log(base);
}

#define log10(a) logab(10, a)

void main() {
	// distance from camera to fragment
	float fragmentDistance = length(FragPos - position);
	//float distanceY = abs(FragPos.y - position.y);
	float targetDistance = length(target - position);
	
	vec4 color4 = vec4(color, 0.0); // start out transparent
	
	// power of 10 of the size (in m) of the largest grid rendered 
	int largestGrid = int(log10(targetDistance));

	// calculate values that represent distance thresholds for generating lines
	float thresholds[LEVELS];
	thresholds[0] = 0.001 * fragmentDistance / pow(10, largestGrid);
	for (int i = 1; i < LEVELS; ++i) {
		//thresholds[i] = sqrt(10) * thresholds[i - 1];
		thresholds[i] = thresholds[i - 1] * 10;
	}

	// declare variabless
	float Xmuls[LEVELS];
	float Zmuls[LEVELS];
	float alphaMuls[LEVELS];
	float Xalphas[LEVELS];
	float Zalphas[LEVELS];

	for (int i = 0; i < LEVELS; ++i) {
		// calculate a multiplier based on size of grid being rendered
		// multiplier = 1 / size of grid in m
		float mult = pow(10, i - largestGrid);

		// calculate a fragment specific multipler based on distance from center
		// of lines
		Xmuls[i] = abs(FragPos.x * mult - round(FragPos.x * mult)) / thresholds[i];
		Zmuls[i] = abs(FragPos.z * mult - round(FragPos.z * mult)) / thresholds[i];

		// calculate an opacity multiplier based on distance
		alphaMuls[i] = min(1.0, 10 / (fragmentDistance * mult));
		
		// calcualte alpha values independent of distance
		Xalphas[i] = exp(-2.7 * Xmuls[i] * Xmuls[i] * Xmuls[i]);
		Zalphas[i] = exp(-2.7 * Zmuls[i] * Zmuls[i] * Zmuls[i]);

		color4 = vec4(color4.xyz, max(color4.w, max(Xalphas[i], Zalphas[i]) * alphaMuls[i]));
	}

	FragColor = color4;
}
