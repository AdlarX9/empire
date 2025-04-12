#version 330 core

#define MAX_LIGHT 16

in vec3  trueCoord;
in vec3  normal;
out vec4 FragColor;

uniform vec3  lightPositions[MAX_LIGHT];
uniform float lightIntensities[MAX_LIGHT];
uniform vec3  lightColors[MAX_LIGHT];
uniform float lightAmbients[MAX_LIGHT];

uniform vec4 objectColor;
uniform uint nbrLights;
uniform vec3 cameraPos;

void main() {
	vec4 color = objectColor;

	float intensity = 0;
	vec3  lightColor = vec3(0, 0, 0);

	vec3 cameraRay = normalize(trueCoord - cameraPos);

	bool behind;
	if (dot(cameraRay, normal) > 0) {
		behind = true;
	} else {
		behind = false;
	}

	int   i;
	float diffusedLight;
	for (i = 0; i < int(nbrLights); i++) {
		if (!behind) {
			lightColor += lightColors[i];
		}

		if (lightAmbients[i] > 0) {
			intensity += lightIntensities[i];
		} else {
			vec3 incidentRay = normalize(trueCoord - lightPositions[i]);
			vec3 reflectedRay = reflect(incidentRay, normal);

			diffusedLight = max(dot(normal, -incidentRay), 0.);
			if (diffusedLight > 0 && !behind) {
				float reflectedLight = pow(max(dot(cameraRay, -reflectedRay), 0.), 5.);
				intensity += (diffusedLight + reflectedLight) * lightIntensities[i] * 0.4;
			}
		}
	}

	color.r *= lightColor.r;
	color.g *= lightColor.g;
	color.b *= lightColor.b;

	color *= intensity;

	FragColor = color;
}
