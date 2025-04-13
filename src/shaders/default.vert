#version 330 core

layout(location = 0) in vec3 coordinates;
layout(location = 1) in vec3 normalVector;

out vec3 trueCoord;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 rotation;

uniform int screenWidth;
uniform int screenHeight;

void main() {
	vec4 position = projection * view * model * vec4(coordinates, 1.);

	position.x *= float(screenHeight) / float(screenWidth);

	gl_Position = position;

	trueCoord = (model * vec4(coordinates, 1.)).xyz;
	normal = normalize(rotation * vec4(normalVector, 1.)).xyz;
}
