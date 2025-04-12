#version 330 core

#define MAX_LIGHT 16

in vec3  trueCoord;
in vec3  normal;
out vec4 FragColor;

uniform vec3  lightPositions[MAX_LIGHT];
uniform float lightIntensities[MAX_LIGHT];
uniform vec3  lightColors[MAX_LIGHT];
uniform float lightAmbients[MAX_LIGHT];

uniform vec3 cameraPos;
uniform uint nbrLights;

uniform vec4  objectColor;
uniform float objectMetalness;

void main() {
	vec4 color = objectColor;

	if (objectMetalness <= 0) {
		FragColor = color;
		return;
	}

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
			lightColor += lightColors[i] * lightIntensities[i];
		}

		if (lightAmbients[i] > 0) {
			intensity += lightIntensities[i] / objectMetalness;
		} else {
			vec3 incidentRay = trueCoord - lightPositions[i];
			float distance = length(incidentRay);
			incidentRay = normalize(incidentRay);
			vec3 reflectedRay = reflect(incidentRay, normal);

			diffusedLight = max(dot(normal, -incidentRay), 0.);
			if (diffusedLight > 0 && !behind) {
				float reflectedLight = pow(max(dot(cameraRay, -reflectedRay), 0.), 5. * objectMetalness);
				intensity += (diffusedLight / objectMetalness + reflectedLight * objectMetalness) * lightIntensities[i] * 0.4 / distance;
			}
		}
	}

	color.r *= lightColor.r;
	color.g *= lightColor.g;
	color.b *= lightColor.b;

	color *= intensity;

	FragColor = color;
}
