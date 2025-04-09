#define WIDTH 2400
#define HEIGHT 1500

#include "../lib/glad/glad.h"
#include "../three/main.hpp"

#include <GLFW/glfw3.h>

#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>

#include "../maths/utils.hpp"

using namespace std;

int main() {
	// Initialisation et création de la fenêtre
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Empire", nullptr, nullptr);
	if (window == nullptr) {
		cerr << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cerr << "Failed to initialize GLAD" << endl;
		return -1;
	}

	// Initialisation de la scène
	glm::vec3 cameraPosition(1, 2, 1);
	Camera    camera(window, cameraPosition);
	camera.lookAt(0, 0, 0);

	Scene    scene;
	Renderer renderer(camera, scene);

	BoxGeometry cubeGeometry;
	Material    cubeMaterial;
	Mesh        cube(cubeGeometry, cubeMaterial);
	scene.add(&cube);

	// Boucle de jeu
	auto         start = std::chrono::high_resolution_clock::now();
	unsigned int nbrFrame = 0;
	auto         localStart = std::chrono::high_resolution_clock::now();

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		nbrFrame++;

		renderer.render();
		glfwSwapBuffers(window);

		auto now = std::chrono::high_resolution_clock::now();
		auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - localStart).count();
		camera.handleKeyControls(deltaTime / 1000.0f);
		camera.handleMouseControls();
		localStart = std::chrono::high_resolution_clock::now();
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	glfwDestroyWindow(window);
	glfwTerminate();

	cout << "Average FPS: " << (double)nbrFrame / duration.count() * 1000 << endl;
	return 0;
}
