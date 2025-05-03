#define WIDTH 2400
#define HEIGHT 1500

#include "../lib/glad/glad.h"
#include "../maths/utils.hpp"
#include "../three/main.hpp"
#include "../physics/main.hpp"

#include <GLFW/glfw3.h>

#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>


using namespace std;




double first3dScene(GLFWwindow* window) {
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
	unsigned int nbrFrame = 0;

	auto start = chrono::high_resolution_clock::now();
	auto localStart = chrono::high_resolution_clock::now();
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

	auto   duration = chrono::duration_cast<chrono::milliseconds>(end - start);
	double fps = (double)nbrFrame / duration.count() * 1000;
	return fps;
}



double firstPhysicsScene(GLFWwindow* window) {
	// Initialisation de la scène
	glm::vec3 cameraPosition(4, 2, 6);
	Camera    camera(window, cameraPosition);
	camera.lookAt(0, 0, 0);

	Planet planet;
	Scene& scene = planet.getScene();

	Renderer renderer(camera, scene);

	BoxGeometry stickGeometry(1, 6);
	Material    stickMaterial1(glm::vec4(0.8, 0.2, 0, 1), 1.5);
	Material    stickMaterial2(glm::vec4(0.2, 0.8, 0, 1), 1.5);
	Mesh        stick1(stickGeometry, stickMaterial1);
	Mesh        stick2(stickGeometry, stickMaterial2);
	stick1.translate(-3, -6.01, 1);

	vector<Mass> masses = {Mass(1, glm::vec3(-0.5, 0.5, 0)), Mass(1, glm::vec3(0.5, 0.5, 0)), Mass(1, glm::vec3(0, -0.5, 0))};
	Solid        solid1(masses);
	Solid        solid2(masses);

	WorldObject worldObject1({}, solid1, stick1);
	WorldObject worldObject2({}, solid2, stick2);

	BallJoint joint(&worldObject1, glm::vec3(0, 3, 0), &worldObject2, glm::vec3(0, -3, 0));

	Skeleton skeleton({&worldObject1, &worldObject2}, {&joint});
	planet.add(&skeleton);

	// Lumières
	PointLight pointLight1(glm::vec3(2, 1, 1), 2);
	scene.add(&pointLight1);
	PointLight pointLight2(glm::vec3(-2, 0, 2), 2);
	scene.add(&pointLight2);
	AmbientLight ambientLight(0.1, glm::vec3(1, 1, 1));
	scene.add(&ambientLight);

	// Boucle de jeu
	unsigned int nbrFrame = 0;

	auto start = chrono::high_resolution_clock::now();
	auto localStart = chrono::high_resolution_clock::now();
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		nbrFrame++;

		worldObject1.applyForce(Force(glm::vec3(0, -3, 0), glm::vec3(-8, 0, 1)));
		worldObject2.applyForce(Force(glm::vec3(0, 3, 0), glm::vec3(8, 0, -1)));

		planet.update();
		renderer.render();
		glfwSwapBuffers(window);

		auto now = chrono::high_resolution_clock::now();
		auto deltaTime = chrono::duration_cast<chrono::milliseconds>(now - localStart).count();
		camera.handleKeyControls(deltaTime / 1000.0f);
		camera.handleMouseControls();
		localStart = chrono::high_resolution_clock::now();
	}
	auto end = chrono::high_resolution_clock::now();

	auto   duration = chrono::duration_cast<chrono::milliseconds>(end - start);
	double fps = (double)nbrFrame / duration.count() * 1000;
	return fps;
}



int main() {
	// Initialisation de la fenêtre
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

	// Lancer la scène
	double fps = firstPhysicsScene(window);
	cout << "Average FPS: " << fps << endl;

	// Nettoyer la fenêtre
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
