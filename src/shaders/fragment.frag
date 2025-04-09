#version 330 core

in vec3 test;
out vec4 FragColor;

void main() {
	FragColor = vec4(.5 - test.z, sqrt(test.x * test.x + test.y * test.y + test.z * test.z), gl_FragCoord.z - 0.2, 1.);
}
