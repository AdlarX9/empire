#version 330 core

layout(location = 0) in vec3 aPos;
out vec3 test;

uniform mat3 cameraBasis;
uniform vec3 cameraPos;

uniform int screenWidth;
uniform int screenHeight;

void main() {
	vec3 position = aPos;

	position -= cameraPos;
	position = cameraBasis * position;

	position.xy /= abs(position.z);
	position.x *= float(screenHeight) / float(screenWidth);

	// Définition de la position finale pour OpenGL
	gl_Position = vec4(position.xy, position.z / 100., 1.0);
	test = aPos;
}
