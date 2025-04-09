#define WIDTH 2400
#define HEIGHT 1500

#include "main.hpp"
#include "../lib/glad/glad.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "../maths/utils.hpp"

using namespace std;




/* --- GEOMETRY --- */



Geometry::Geometry(unsigned int vertexCount, GLfloat* vertices, unsigned int faceCount, GLuint* faces, GLfloat* normalVectors)
    : m_vertexCount(vertexCount), m_vertices(vertices), m_faceCount(faceCount), m_faces(faces), m_normalVectors(normalVectors) {}

unsigned int Geometry::vertexCount() const { return m_vertexCount; }
GLfloat*     Geometry::getVertices() const { return m_vertices; }
unsigned int Geometry::faceCount() const { return m_faceCount; }
GLuint*      Geometry::getFaces() const { return m_faces; }
GLfloat*&    Geometry::getNormalVectors() { return m_normalVectors; }

Geometry::~Geometry() {}



/* --- MATERIAL --- */



Material::Material(glm::vec4 color) : m_mainColor(color) {}

glm::vec4& Material::getMainColor() { return m_mainColor; }

Material& Material::setMainColor(glm::vec4 color) {
	m_mainColor = color;
	return *this;
}

Material::~Material() {}



/* --- MESH --- */



Mesh::Mesh(Geometry& geometry, Material& material) : m_geometry(geometry), m_material(material) {}

Geometry& Mesh::getGeometry() { return m_geometry; }
Material& Mesh::getMaterial() { return m_material; }

Mesh::~Mesh() {}



/* --- SCENE --- */



Scene::Scene() : m_meshes(vector<Mesh*>()) {}

vector<Mesh*>& Scene::getMeshes() { return m_meshes; }

Scene& Scene::add(Mesh* mesh) {
	m_meshes.push_back(mesh);
	return *this;
}

Scene& Scene::remove(Mesh* mesh) {
	auto it = find(m_meshes.begin(), m_meshes.end(), mesh);
	if (it != m_meshes.end()) {
		m_meshes.erase(it);
	}
	return *this;
}

Scene::~Scene() {}



/* --- CAMERA --- */



Camera::Camera(GLFWwindow* window, glm::vec3 position, unsigned int fov, glm::vec3 direction)
    : m_window(window),
      m_position(position),
      m_defaultDirection(glm::vec3(0, 0, 1)),
      m_fov(fov),
      m_direction(direction),
      m_mousePos(glm::vec2(0, 0)) {}

glm::vec3&   Camera::getPosition() { return m_position; }
glm::vec3&   Camera::getDefaultDirection() { return m_defaultDirection; }
glm::vec3&   Camera::getDirection() { return m_direction; }
unsigned int Camera::getFov() const { return m_fov; }

glm::vec3 Camera::getXAxis() const {
	glm::vec3 xAxis = glm::cross(m_direction, m_defaultDirection);
	if (xAxis == glm::vec3(0, 0, 0)) {
		xAxis = glm::vec3(1, 0, 0);
	}
	xAxis = -glm::normalize(xAxis);
	return xAxis;
}

glm::vec3 Camera::getYAxis() const {
	glm::vec3 yAxis = glm::cross(m_direction, this->getXAxis());
	if (yAxis == glm::vec3(0, 0, 0)) {
		yAxis = glm::vec3(0, 1, 0);
	}
	yAxis = glm::normalize(yAxis);
	return yAxis;
}

Camera& Camera::lookAt(const glm::vec3& point) {
	m_direction = glm::normalize(point - m_position);
	return *this;
}

Camera& Camera::lookAt(double x, double y, double z) {
	glm::vec3 point(x, y, z);
	this->lookAt(point);
	return *this;
}

Camera& Camera::translate(double x, double y, double z) {
	m_position += glm::vec3(x, y, z);
	return *this;
}

void Camera::handleKeyControls(float deltaTime) {
	float speed = 2.0f;
	float rotationSpeed = 40.0f;
	float adjustment = 2e17;
	if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) {
		m_position += this->getDirection() * speed * deltaTime;
	}
	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) {
		m_position += this->getDirection() * -speed * deltaTime;
	}
	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) {
		m_position += this->getXAxis() * -speed * deltaTime;
	}
	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) {
		m_position += this->getXAxis() * speed * deltaTime;
	}
	if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		m_position += this->getYAxis() * speed * deltaTime;
	}
	if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		m_position += this->getYAxis() * -speed * deltaTime;
	}
}

void Camera::handleMouseControls() {
	if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS) {
		m_mousePos.x = -1;
		m_mousePos.y = -1;
		return;
	}
	if (m_mousePos.x < 0 && m_mousePos.y < 0) {
		double xpos, ypos;
		glfwGetCursorPos(m_window, &xpos, &ypos);
		m_mousePos.x = xpos;
		m_mousePos.y = ypos;
		return;
	}

	double xpos, ypos;
	glfwGetCursorPos(m_window, &xpos, &ypos);

	float xoffset = m_mousePos.x - xpos;
	float yoffset = ypos - m_mousePos.y;

	float sensitivity = 0.002f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	glm::vec3 xAxis = this->getXAxis();
	glm::vec3 yAxis = this->getYAxis();

	m_direction -= xAxis * xoffset + yAxis * yoffset;
	m_direction = glm::normalize(m_direction);
	m_mousePos.x = xpos;
	m_mousePos.y = ypos;
}

Camera::~Camera() {}



/* --- RENDERER --- */



Renderer::Renderer(Camera& camera, Scene& scene) : m_camera(camera), m_scene(scene), m_shaderProgram(0) {
	string vertexString = loadShader("src/shaders/vertex.vert");
	string fragmentString = loadShader("src/shaders/fragment.frag");

	const char* vertexShaderSource = vertexString.c_str();
	const char* fragmentShaderSource = fragmentString.c_str();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);

	m_shaderProgram = glCreateProgram();

	glAttachShader(m_shaderProgram, vertexShader);
	glAttachShader(m_shaderProgram, fragmentShader);
	glLinkProgram(m_shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

string Renderer::loadShader(const char* path) {
	ifstream in(path, ios::binary);

	if (in) {
		string contents;
		in.seekg(0, ios::end);
		contents.resize(in.tellg());
		in.seekg(0, ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return contents;
	} else {
		cerr << "Error: Could not open shader file " << path << endl;
		return "";
	}
}

void Renderer::render() {
	for (Mesh* mesh : m_scene.getMeshes()) {
		Geometry& geometry = mesh->getGeometry();
		Material& material = mesh->getMaterial();

		GLfloat* vertices = geometry.getVertices();
		GLuint*  faces = geometry.getFaces();
		GLfloat* normal = geometry.getNormalVectors();

		glm::vec4 color = material.getMainColor();

		GLuint VAO, VBO, EBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		size_t vertexCount = geometry.vertexCount();
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(float), vertices, GL_STATIC_DRAW);

		size_t faceCount = geometry.faceCount();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceCount * 3 * sizeof(GLuint), faces, GL_STATIC_DRAW);

		// Envoi des attributs de vertex
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		// glEnableVertexAttribArray(2);

		// Envoi des uniform
		glUseProgram(m_shaderProgram);

		glm::vec3 xAxis = m_camera.getXAxis();
		glm::vec3 yAxis = m_camera.getYAxis();
		glm::vec3 zAxis = m_camera.getDirection();

		glm::mat3 cameraBasis(xAxis, yAxis, zAxis);

		cameraBasis = glm::transpose(cameraBasis);

		// Envoi au shader
		GLuint basisLoc = glGetUniformLocation(m_shaderProgram, "cameraBasis");
		glUniformMatrix3fv(basisLoc, 1, GL_FALSE, glm::value_ptr(cameraBasis));
		GLuint cameraPosLoc = glGetUniformLocation(m_shaderProgram, "cameraPos");
		glUniform3f(cameraPosLoc, m_camera.getPosition().x, m_camera.getPosition().y, m_camera.getPosition().z);

		GLuint screenWidthLoc = glGetUniformLocation(m_shaderProgram, "screenWidth");
		glUniform1i(screenWidthLoc, WIDTH);
		GLuint screenHeightLoc = glGetUniformLocation(m_shaderProgram, "screenHeight");
		glUniform1i(screenHeightLoc, HEIGHT);

		// Rendu
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, geometry.faceCount() * 3, GL_UNSIGNED_INT, 0);

		// Supprimer les buffers et le VAO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}
}

Renderer::~Renderer() { glDeleteProgram(m_shaderProgram); }



/* --- BOXGEOMETRY --- */



BoxGeometry::BoxGeometry(double x, double y, double z) : Geometry::Geometry(8, nullptr, 12, nullptr, nullptr) {
	m_vertexCount = 8;
	m_faceCount = 12;

	GLfloat halfX = x / 2.0f;
	GLfloat halfY = y / 2.0f;
	GLfloat halfZ = z / 2.0f;

	// Définition des vertices du cube
	static vector<GLfloat> vertices = {
	    -halfX, -halfY, -halfZ,  // 0 - arrière-gauche-bas
	    halfX,  -halfY, -halfZ,  // 1 - arrière-droit-bas
	    halfX,  halfY,  -halfZ,  // 2 - avant-droit-bas
	    -halfX, halfY,  -halfZ,  // 3 - avant-gauche-bas
	    -halfX, halfY,  halfZ,   // 4 - avant-gauche-haut
	    -halfX, -halfY, halfZ,   // 5 - arrière-gauche-haut
	    halfX,  -halfY, halfZ,   // 6 - arrière-droit-haut
	    halfX,  halfY,  halfZ    // 7 - avant-droit-haut
	};
	m_vertices = vertices.data();

	// Définition des indices des faces
	static GLuint faces[] = {
	    0, 1, 2, 0, 2, 3,  // Face arrière
	    4, 5, 6, 4, 6, 7,  // Face avant
	    0, 1, 5, 0, 5, 4,  // Face gauche
	    2, 3, 7, 2, 7, 6,  // Face droite
	    0, 3, 4, 3, 4, 7,  // Face bas
	    1, 2, 6, 1, 6, 5   // Face haut
	};
	m_faces = faces;

	// Définition des normales des faces
	static GLfloat normalVectors[] = {
	    0,  0,  -1,  // Normale face arrière
	    0,  0,  -1,  // Normale face arrière
	    0,  0,  1,   // Normale face avant
	    0,  0,  1,   // Normale face avant
	    -1, 0,  0,   // Normale face gauche
	    -1, 0,  0,   // Normale face gauche
	    1,  0,  0,   // Normale face droite
	    1,  0,  0,   // Normale face droite
	    0,  -1, 0,   // Normale face bas
	    0,  -1, 0,   // Normale face bas
	    0,  1,  0,   // Normale face haut
	    0,  1,  0    // Normale face haut
	};
	m_normalVectors = normalVectors;
}

BoxGeometry::~BoxGeometry() {}
