#define WIDTH 2400
#define HEIGHT 1500

#include "main.hpp"
#include "../lib/glad/glad.h"

#include <glm/glm.hpp>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "../maths/utils.hpp"

using namespace std;




/* --- GEOMETRY --- */



Geometry::Geometry(unsigned int vertexCount, GLfloat* vertices, GLuint* faces, GLfloat* normalVectors)
    : m_vertexCount(vertexCount), m_vertices(vertices), m_faces(faces), m_normalVectors(normalVectors) {}

unsigned int Geometry::vertexCount() const { return m_vertexCount; }
GLfloat*&    Geometry::getVertices() { return m_vertices; }
GLuint*&     Geometry::getFaces() { return m_faces; }
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



Camera::Camera(glm::vec3 position, unsigned int fov, glm::vec3 direction)
    : m_position(position), m_defaultDirection(direction), m_fov(fov), m_rotation(UnitQuaternion()) {}

glm::vec3&      Camera::getPosition() { return m_position; }
glm::vec3&      Camera::getDefaultDirection() { return m_defaultDirection; }
UnitQuaternion& Camera::getRotation() { return m_rotation; }
unsigned int    Camera::getFov() const { return m_fov; }

Camera& Camera::lookAt(glm::vec3& point) {
	glm::vec3 direction = point - m_position;
	direction = glm::normalize(direction);

	glm::vec3 defaultDirection = m_defaultDirection;
	defaultDirection = glm::normalize(defaultDirection);

	double dotProduct = glm::dot(defaultDirection, direction);
	dotProduct = std::clamp(dotProduct, -1.0, 1.0);
	double angle = acos(dotProduct);

	Quaternion axis = Quaternion(defaultDirection) * Quaternion(direction);

	if (axis.length() < 1e-6) {
		if (dotProduct < 0) {
			axis = Quaternion(1, 0, 0);
		} else {
			return *this;
		}
	}

	axis.normalize();
	m_rotation = UnitQuaternion(angle, axis.x(), axis.y(), axis.z());

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

Camera::~Camera() {}



/* --- RENDERER --- */



Renderer::Renderer(Camera& camera, Scene& scene) : m_camera(camera), m_scene(scene), m_shaderProgram(0) {
	std::string vertexShaderString = loadShader("src/shaders/rasterization.vert");
	std::string fragmentShaderString = loadShader("src/shaders/mapping.frag");
	const char* vertexShaderSource = vertexShaderString.c_str();
	const char* fragmentShaderSource = fragmentShaderString.c_str();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);

	GLuint shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

std::string Renderer::loadShader(const char* path) {
	std::ifstream in(path, std::ios::binary);

	if (in) {
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return contents;
	} else {
		std::cerr << "Error: Could not open shader file " << path << std::endl;
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

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(faces), faces, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glUseProgram(m_shaderProgram);
		glBindVertexArray(VAO);
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}
}

Renderer::~Renderer() { glDeleteProgram(m_shaderProgram); }



/* --- BOXGEOMETRY --- */



BoxGeometry::BoxGeometry(double x, double y, double z) : Geometry::Geometry(0, {}, {}, {}) {
	m_vertexCount = 8;

	GLfloat halfX = x / 2.0;
	GLfloat halfY = y / 2.0;
	GLfloat halfZ = z / 2.0;

	GLfloat vertices[] = {
	    -halfX, -halfY, -halfZ,  //
	    halfX,  -halfY, -halfZ,  //
	    halfX,  halfY,  -halfZ,  //
	    -halfX, halfY,  -halfZ,  //
	    -halfX, halfY,  halfZ,   //
	    -halfX, -halfY, halfZ,   //
	    halfX,  -halfY, halfZ,   //
	    halfX,  halfY,  halfZ    //
	};
	m_vertices = vertices;

	GLuint faces[] = {0, 1, 2, 0, 2, 3, 1, 2, 7, 1, 2, 6, 1, 0, 5, 1, 0, 6, 4, 5, 6, 4, 5, 7, 4, 3, 0, 4, 3, 5, 4, 7, 2, 4, 7, 3};
	m_faces = faces;

	GLfloat normalVectors[] = {
	    0,  0,  -1,  //
	    0,  0,  -1,  //
	    1,  0,  0,   //
	    1,  0,  0,   //
	    0,  -1, 0,   //
	    0,  -1, 0,   //
	    0,  0,  1,   //
	    0,  0,  1,   //
	    -1, 0,  0,   //
	    -1, 0,  0,   //
	    0,  1,  0,   //
	    0,  1,  0,   //
	};
	m_normalVectors = normalVectors;
}

BoxGeometry::~BoxGeometry() {}
