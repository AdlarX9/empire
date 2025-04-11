#version 330 core

layout(location = 0) in vec3 Coordinates;
layout(location = 1) in vec3 normalVector;

out vec3 trueCoord;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform int screenWidth;
uniform int screenHeight;

void main() {
	vec4 position = projection * view * model * vec4(Coordinates, 1.);

	position.x *= float(screenHeight) / float(screenWidth);

	gl_Position = position;

	trueCoord = Coordinates;
	normal = normalize(normalVector);
}
