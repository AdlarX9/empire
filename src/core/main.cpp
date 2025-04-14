#define WIDTH 2400
#define HEIGHT 1500

#include "../lib/glad/glad.h"
#include "../three/main.hpp"

#include <GLFW/glfw3.h>

#include <chrono>
#include <glm/glm.hpp>
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
	camera.lookAt(0, 0, 1.2);

	Scene    scene;
	Renderer renderer(camera, scene);

	SphereGeometry sphereGeometry(1, 100, 100);
	LinesMaterial  sphereMaterial(glm::vec4(0.2, 1, 0, 1));
	Mesh           sphere(sphereGeometry, sphereMaterial);
	scene.add(&sphere);

	sphere.translate(0, 0, 1.2).rotateSelf(90, glm::vec3(0, 1, 0)).rotateScene(45, glm::vec3(0, 0, 1));

	PlaneGeometry planeGeometry(5, 5);
	Material      planeMaterial(0.607, 0.552, 0.447);
	Mesh          plane(planeGeometry, planeMaterial);
	scene.add(&plane);

	PointLight pointLight1(2, 1, 1);
	scene.add(&pointLight1);

	PointLight pointLight2(-2, 0, 2, 2);
	scene.add(&pointLight2);

	AmbientLight ambientLight(0.1, glm::vec3(1, 1, 1));
	scene.add(&ambientLight);

	// Boucle de jeu
	auto         start = chrono::high_resolution_clock::now();
	unsigned int nbrFrame = 0;
	auto         localStart = chrono::high_resolution_clock::now();

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		nbrFrame++;

		renderer.render();
		glfwSwapBuffers(window);

		auto now = chrono::high_resolution_clock::now();
		auto deltaTime = chrono::duration_cast<chrono::milliseconds>(now - localStart).count();
		camera.handleKeyControls(deltaTime / 1000.0f);
		camera.handleMouseControls();
		localStart = chrono::high_resolution_clock::now();
	}

	auto end = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

	glfwDestroyWindow(window);
	glfwTerminate();

	cout << "Average FPS: " << (double)nbrFrame / duration.count() * 1000 << endl;
	return 0;
}
