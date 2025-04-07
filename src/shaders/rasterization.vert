#version 330 core

layout(location = 0) in vec3 aPos;
// layout(location = 1) in vec3 aColor;
// out vec4 color;

void main() {
	// color = vec4(aColor, 1.);
	vec4 position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	gl_Position = position;
}
