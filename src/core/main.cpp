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
	GLFWwindow* window = glfwCreateWindow(1280, 800, "Empire", nullptr, nullptr);
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
	glm::vec3 cameraPosition(1, 1, 1);
	Camera    camera(cameraPosition);
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

	while (!glfwWindowShouldClose(window)) {
		renderer.render();
		glfwSwapBuffers(window);

		glfwPollEvents();
		nbrFrame++;
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);

	glfwDestroyWindow(window);
	glfwTerminate();

	cout << "Average FPS: " << (double)nbrFrame / duration.count() << endl;
	return 0;
}
