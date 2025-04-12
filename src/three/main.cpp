#define WIDTH 2400
#define HEIGHT 1500

#define MAX_LIGHT 16

#include "main.hpp"
#include "../opengl/main.hpp"
#include "../maths/utils.hpp"
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



Material::Material(glm::vec4 color, float metalness) : m_mainColor(color), m_metalness(metalness) {}
Material::Material(float r, float g, float b, float a, float metalness) : m_mainColor(glm::vec4(r, g, b, a)), m_metalness(metalness) {}

glm::vec4& Material::getMainColor() { return m_mainColor; }

Material& Material::setMainColor(glm::vec4 color) {
	m_mainColor = color;
	return *this;
}

float Material::getMetalness() const { return m_metalness; }

Material& Material::setMetalness(float metalness) {
	m_metalness = metalness;
	return *this;
}

void Material::finalRender(unsigned int faceCount) const { glDrawElements(GL_TRIANGLES, faceCount * 3, GL_UNSIGNED_INT, 0); }

GLuint* Material::getFacesData(unsigned int faceCount) const {
	GLuint* facesData = new GLuint[faceCount * 3];

	for (unsigned int i = 0; i < faceCount * 3; i++) {
		facesData[i] = i;
	}

	return facesData;
}

Material::~Material() {}



/* --- MESH --- */



Mesh::Mesh(Geometry& geometry, Material& material)
    : m_geometry(geometry), m_material(material), m_rotation(UnitQuaternion()), m_translation(glm::vec3(0, 0, 0)), m_scale(1.0f) {}

Geometry&       Mesh::getGeometry() { return m_geometry; }
Material&       Mesh::getMaterial() { return m_material; }
UnitQuaternion& Mesh::getRotation() { return m_rotation; }
glm::vec3&      Mesh::getTranslation() { return m_translation; }
glm::vec3&      Mesh::getScale() { return m_scale; }

GLfloat* Mesh::getVerticesData() const {
	unsigned int nbrVectors = 2;  // Vertices + normales
	GLfloat*     data = new GLfloat[m_geometry.faceCount() * 9 * nbrVectors];

	GLfloat* vertices = m_geometry.getVertices();
	GLfloat* normalVectors = m_geometry.getNormalVectors();
	GLuint*  faces = m_geometry.getFaces();

	unsigned int index = 0;

	for (unsigned int i = 0; i < m_geometry.faceCount(); i++) {
		for (unsigned int j = 0; j < 3; j++) {
			unsigned int vertexIndex = faces[i * 3 + j];
			data[index] = vertices[vertexIndex * 3];          // x
			data[index + 1] = vertices[vertexIndex * 3 + 1];  // y
			data[index + 2] = vertices[vertexIndex * 3 + 2];  // z

			unsigned int normalIndex = i;
			data[index + 3] = normalVectors[normalIndex * 3];      // nx
			data[index + 4] = normalVectors[normalIndex * 3 + 1];  // ny
			data[index + 5] = normalVectors[normalIndex * 3 + 2];  // nz

			index += 6;
		}
	}

	return data;
}

Mesh::~Mesh() {}



/* --- LIGHT --- */



Light::Light(glm::vec3 position, double intensity, glm::vec3 color, bool ambient)
    : m_position(position), m_intensity(intensity), m_color(color), m_ambient(ambient) {}


glm::vec3& Light::getPosition() { return m_position; }
float      Light::getIntensity() const { return m_intensity; }
glm::vec3& Light::getColor() { return m_color; }
bool       Light::getAmbient() const { return m_ambient; }


Light::~Light() {}



/* --- SCENE --- */



Scene::Scene(glm::vec3 backgroundColor) : m_meshes(vector<Mesh*>()), m_lights(vector<Light*>()), m_backgroundColor(backgroundColor) {}
Scene::Scene(float r, float g, float b) : m_meshes(vector<Mesh*>()), m_lights(vector<Light*>()), m_backgroundColor(glm::vec3(r, g, b)) {}

vector<Mesh*>&  Scene::getMeshes() { return m_meshes; }
glm::vec3&      Scene::getBackGroundColor() { return m_backgroundColor; }
unsigned int    Scene::getNbrLights() const { return m_lights.size(); }
vector<Light*>& Scene::getLights() { return m_lights; }

Scene& Scene::setBackGroundColor(glm::vec3 backgroundColor) {
	m_backgroundColor = backgroundColor;
	return *this;
}

Scene& Scene::setBackGroundColor(float r, float g, float b) {
	m_backgroundColor = glm::vec3(r, g, b);
	return *this;
}

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

Scene& Scene::add(Light* light) {
	m_lights.push_back(light);
	return *this;
}

Scene& Scene::remove(Light* light) {
	auto it = find(m_lights.begin(), m_lights.end(), light);
	if (it != m_lights.end()) {
		m_lights.erase(it);
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
		m_position += this->getXAxis() * speed * deltaTime;
	}
	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) {
		m_position += this->getXAxis() * -speed * deltaTime;
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
	float yoffset = m_mousePos.y - ypos;

	float sensitivity = 0.002f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	glm::vec3 xAxis = this->getXAxis();
	glm::vec3 yAxis = this->getYAxis();

	m_direction += xAxis * xoffset + yAxis * yoffset;
	m_direction = glm::normalize(m_direction);
	m_mousePos.x = xpos;
	m_mousePos.y = ypos;
}

Camera::~Camera() {}



/* --- RENDERER --- */



Renderer::Renderer(Camera& camera, Scene& scene)
    : m_camera(camera), m_scene(scene), m_shader(Shader("src/shaders/default.vert", "src/shaders/default.frag")) {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

void Renderer::initializeUniforms() {
	m_shader.addUniform("screenWidth", WIDTH);
	m_shader.addUniform("screenHeight", HEIGHT);
	m_shader.addUniform("cameraPos", m_camera.getPosition());

	glm::mat4 view = glm::lookAt(m_camera.getPosition(), m_camera.getPosition() + m_camera.getDirection(), glm::vec3(0, 0, 1));
	glm::mat4 projection = glm::perspective((float)glm::radians((float)m_camera.getFov()), (float)(WIDTH / HEIGHT), 0.1f, 100.0f);
	m_shader.addUniform("view", view);
	m_shader.addUniform("projection", projection);


	// Envoyer les lumières
	m_shader.addUniform("nbrLights", m_scene.getNbrLights());

	vector<Light*> lights = m_scene.getLights();
	glm::vec3      positions[MAX_LIGHT];
	GLfloat        intensities[MAX_LIGHT];
	glm::vec3      colors[MAX_LIGHT];
	float          ambients[MAX_LIGHT];

	for (unsigned int i = 0; i < lights.size(); i++) {
		positions[i] = lights[i]->getPosition();
		intensities[i] = lights[i]->getIntensity();
		colors[i] = lights[i]->getColor();
		ambients[i] = lights[i]->getAmbient() ? 1 : 0;
	}

	for (unsigned int i = lights.size(); i < MAX_LIGHT; i++) {
		positions[i] = glm::vec3(0, 0, 0);
		intensities[i] = 0;
		colors[i] = glm::vec3(1, 1, 1);
		ambients[i] = false;
	}

	m_shader.addUniform("lightPositions", positions, MAX_LIGHT);
	m_shader.addUniform("lightIntensities", intensities, MAX_LIGHT);
	m_shader.addUniform("lightColors", colors, MAX_LIGHT);
	m_shader.addUniform("lightAmbients", ambients, MAX_LIGHT);
}

void Renderer::clearScreen() {
	glm::vec3 backgroundColor = m_scene.getBackGroundColor();
	glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


// Script de rendu
void Renderer::render() {
	m_shader.use();
	this->initializeUniforms();
	this->clearScreen();

	for (Mesh* mesh : m_scene.getMeshes()) {
		// Récupérer les données nécessaires
		Geometry& geometry = mesh->getGeometry();
		Material& material = mesh->getMaterial();
		GLfloat*  verticesData = mesh->getVerticesData();
		GLuint*   facesData = material.getFacesData(geometry.faceCount());
		glm::vec4 color = material.getMainColor();

		// Envoi des uniform personnalisées à l'objet traité
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, mesh->getTranslation());
		model = glm::rotate(model, mesh->getRotation().getAngle(), mesh->getRotation().getAxis());
		model = glm::scale(model, mesh->getScale());
		m_shader.addUniform("model", model);
		m_shader.addUniform("objectColor", material.getMainColor());
		m_shader.addUniform("objectMetalness", material.getMetalness());

		// Création des objets OpenGL
		VAO VAO;
		VBO VBO;
		EBO EBO;

		size_t faceCount = geometry.faceCount();
		VAO.bind();
		VBO.bind(faceCount * 3 * 6, verticesData);
		EBO.bind(faceCount * 3, facesData);

		EBO.addAttribute(0, 3, 0, 6);
		EBO.addAttribute(1, 3, 3, 6);

		// Rendu
		material.finalRender(geometry.faceCount());

		// Supprimer les données allouées
		VAO.unBind();
		VBO.unBind();
		EBO.unBind();

		delete[] verticesData;
		delete[] facesData;
	}
}

Renderer::~Renderer() {}



/* --- BOXGEOMETRY --- */



BoxGeometry::BoxGeometry(double x, double y, double z) : Geometry::Geometry(8, nullptr, 12, nullptr, nullptr) {
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
	    0, 1, 2, 0, 2, 3,  // Face bas
	    4, 5, 6, 4, 6, 7,  // Face haut
	    0, 4, 3, 0, 5, 4,  // Face gauche
	    2, 6, 1, 2, 7, 6,  // Face droite
	    3, 7, 2, 3, 4, 7,  // Face arrière
	    1, 5, 0, 1, 6, 5   // Face avant
	};
	m_faces = faces;

	// Définition des normales des faces
	static GLfloat normalVectors[] = {
	    0,  0,  -1,  // Normale face bas
	    0,  0,  -1,  // Normale face bas
	    0,  0,  1,   // Normale face haut
	    0,  0,  1,   // Normale face haut
	    -1, 0,  0,   // Normale face gauche
	    -1, 0,  0,   // Normale face gauche
	    1,  0,  0,   // Normale face droite
	    1,  0,  0,   // Normale face droite
	    0,  1,  0,   // Normale face arrière
	    0,  1,  0,   // Normale face arrière
	    0,  -1, 0,   // Normale face avant
	    0,  -1, 0    // Normale face avant
	};
	m_normalVectors = normalVectors;
}

BoxGeometry::~BoxGeometry() {}



/* --- PLANEGEOMETRY --- */



PlaneGeometry::PlaneGeometry(double x, double y) : Geometry::Geometry(4, nullptr, 2, nullptr, nullptr) {
	m_vertexCount = 4;
	m_faceCount = 2;

	GLfloat halfX = x / 2.0f;
	GLfloat halfY = y / 2.0f;

	static vector<GLfloat> vertices = {-halfX, -halfY, 0, -halfX, halfY, 0, halfX, halfY, 0, halfX, -halfY, 0};
	m_vertices = vertices.data();

	static GLuint faces[] = {0, 1, 2, 0, 2, 3};
	m_faces = faces;

	static GLfloat normalVectors[] = {0, 0, 1, 0, 0, 1};
	m_normalVectors = normalVectors;
}

PlaneGeometry::~PlaneGeometry() {}



/* --- PYRAMIDGEOMETRY --- */



PyramidGeometry::PyramidGeometry(float x, float y, float h) : Geometry::Geometry(5, nullptr, 6, nullptr, nullptr) {
	GLfloat halfX = x / 2.0f;
	GLfloat halfY = y / 2.0f;
	GLfloat halfH = h / 2.0f;

	static vector<GLfloat> vertices = {
	    -halfX, -halfY, -halfH,  //
	    halfX,  -halfY, -halfH,  //
	    halfX,  halfY,  -halfH,  //
	    -halfX, halfY,  -halfH,  //
	    0,      0,      halfH    //
	};
	m_vertices = vertices.data();

	static GLuint faces[] = {
	    0, 1, 2, 0, 2, 3,  // face bas
	    0, 1, 4,           // face avant
	    2, 3, 4,           // face arrière
	    0, 3, 4,           // face gauche
	    1, 2, 4            // face droite
	};
	m_faces = faces;

	// Définition des normales des faces
	static GLfloat normalVectors[] = {
	    0,  0,  -1, 0, 0, -1,  // face bas
	    0,  -h, y,             // face avant
	    0,  h,  y,             // face arrière
	    -h, 0,  x,             // face gauche
	    h,  0,  x,             // face droite
	};
	m_normalVectors = normalVectors;
}

PyramidGeometry::~PyramidGeometry() {}



/* --- TETRAHEDRONGEOMETRY --- */



TetrahedronGeometry::TetrahedronGeometry() : Geometry::Geometry(4, nullptr, 4, nullptr, nullptr) {
	float a = 1.0f;
	float sqrt_3 = std::sqrt(3.0f);
	float sqrt_6 = std::sqrt(6.0f);

	glm::vec3 v0(1, 1, 1);
	glm::vec3 v1(-1, -1, 1);
	glm::vec3 v2(-1, 1, -1);
	glm::vec3 v3(1, -1, -1);

	static std::vector<GLfloat> vertices = {
	    v0.x, v0.y, v0.z,  // Sommet 0
	    v1.x, v1.y, v1.z,  // Sommet 1
	    v2.x, v2.y, v2.z,  // Sommet 2
	    v3.x, v3.y, v3.z,  // Sommet 3
	};
	for (auto& v : vertices) v *= a / std::sqrt(2);
	m_vertices = vertices.data();

	static GLuint faces[] = {
	    0, 1, 2,  //
	    0, 3, 1,  //
	    0, 2, 3,  //
	    1, 3, 2   //
	};
	m_faces = faces;

	glm::vec3 nv0 = glm::cross((v0 - v1), (v2 - v0));
	glm::vec3 nv1 = glm::cross((v1 - v0), (v3 - v0));
	glm::vec3 nv2 = glm::cross((v3 - v0), (v2 - v0));
	glm::vec3 nv3 = glm::cross((v2 - v1), (v3 - v1));

	// Normales précises recalculées par produit vectoriel (approximatives ici)
	static std::vector<GLfloat> normalVectors = {
	    nv0.x, nv0.y, nv0.z,  // face 0-1-2
	    nv1.x, nv1.y, nv1.z,  // face 0-3-1
	    nv2.x, nv2.y, nv2.z,  // face 0-2-3
	    nv3.x, nv3.y, nv3.z,  // face 1-3-2
	};
	m_normalVectors = normalVectors.data();
}

TetrahedronGeometry::~TetrahedronGeometry() {}



/* --- TETRAHEDRONGEOMETRY --- */



SphereGeometry::SphereGeometry(float radius, unsigned int verticals, unsigned int rows)
    : Geometry::Geometry(0, nullptr, 0, nullptr, nullptr) {
	static std::vector<GLfloat> vertices;
	for (unsigned int i = 0; i < rows; i++) {
		float vAngle = (i + 1) * M_PI / (rows + 1);  // Angle vertical
		for (unsigned int j = 0; j < verticals; j++) {
			float     hAngle = j * 2 * M_PI / verticals;  // Angle horizontal
			glm::mat4 rotationZ = glm::rotate(glm::mat4(1), hAngle, glm::vec3(0, 0, 1));
			glm::mat4 rotationX = glm::rotate(glm::mat4(1), vAngle, glm::vec3(1, 0, 0));
			glm::mat4 rotation = rotationZ * rotationX;

			glm::vec3 initialVector(0, 0, 1);  // Vecteur initial sur l'axe Y
			glm::vec4 vertex = glm::normalize(rotation * glm::vec4(initialVector, 1)) * radius * 1.414f;

			vertices.push_back(vertex.x);
			vertices.push_back(vertex.y);
			vertices.push_back(vertex.z);
		}
	}
	vertices.push_back(0);
	vertices.push_back(0);
	vertices.push_back(radius);
	vertices.push_back(0);
	vertices.push_back(0);
	vertices.push_back(-radius);

	m_vertexCount = vertices.size();
	m_vertices = vertices.data();

	static std::vector<GLuint>  faces;
	static std::vector<GLfloat> normalVectors;

	// Génération des faces
	for (unsigned int i = 0; i < rows - 1; i++) {
		for (unsigned int j = 0; j < verticals; j++) {
			// Calcul des indices pour la première face
			GLuint first = i * verticals + j;
			GLuint second = i * verticals + (j + 1) % verticals;
			GLuint third = (i + 1) * verticals + j;

			faces.push_back(first);
			faces.push_back(second);
			faces.push_back(third);

			// Vecteurs pour calcul des normales
			glm::vec3 firstVertex(vertices[first * 3], vertices[first * 3 + 1], vertices[first * 3 + 2]);
			glm::vec3 secondVertex(vertices[second * 3], vertices[second * 3 + 1], vertices[second * 3 + 2]);
			glm::vec3 thirdVertex(vertices[third * 3], vertices[third * 3 + 1], vertices[third * 3 + 2]);

			glm::vec3 normal = glm::cross(thirdVertex - firstVertex, secondVertex - firstVertex);
			normal = glm::normalize(normal);

			normalVectors.push_back(normal.x);
			normalVectors.push_back(normal.y);
			normalVectors.push_back(normal.z);

			// Calcul des indices pour la deuxième face
			first = third;
			third = (i + 1) * verticals + (j + 1) % verticals;

			faces.push_back(first);
			faces.push_back(second);
			faces.push_back(third);

			// Vecteurs pour calcul des normales
			firstVertex = glm::vec3(vertices[first * 3], vertices[first * 3 + 1], vertices[first * 3 + 2]);
			secondVertex = glm::vec3(vertices[second * 3], vertices[second * 3 + 1], vertices[second * 3 + 2]);
			thirdVertex = glm::vec3(vertices[third * 3], vertices[third * 3 + 1], vertices[third * 3 + 2]);

			normal = glm::cross(thirdVertex - firstVertex, secondVertex - firstVertex);
			normal = glm::normalize(normal);

			normalVectors.push_back(normal.x);
			normalVectors.push_back(normal.y);
			normalVectors.push_back(normal.z);
		}
	}

	// haut et bas
	for (unsigned h = 0; h < 2; h++) {
		for (unsigned int i = 0; i < verticals; i++) {
			GLuint first = verticals * rows + h;
			GLuint second;
			GLuint third;
			if (h == 1) {
				second = verticals * (rows - 1) + i;
				third = verticals * (rows - 1) + (i + 1) % verticals;
			} else {
				second = i;
				third = (i + 1) % verticals;
			}

			faces.push_back(first);
			faces.push_back(second);
			faces.push_back(third);
			glm::vec3 firstVertex(vertices[first * 3], vertices[first * 3 + 1], vertices[first * 3 + 2]);
			glm::vec3 secondVertex(vertices[second * 3], vertices[second * 3 + 1], vertices[second * 3 + 2]);
			glm::vec3 thirdVertex(vertices[third * 3], vertices[third * 3 + 1], vertices[third * 3 + 2]);
			glm::vec3 normal = glm::cross(thirdVertex - firstVertex, secondVertex - firstVertex);
			if (h == 0) {
				normal *= -1;
			}
			normal = glm::normalize(normal);
			normalVectors.push_back(normal.x);
			normalVectors.push_back(normal.y);
			normalVectors.push_back(normal.z);
		}
	}

	m_faceCount = faces.size() / 3;
	m_faces = faces.data();
	m_normalVectors = normalVectors.data();
}


SphereGeometry::~SphereGeometry() {}



/* --- BASICMATERIAL --- */



BasicMaterial::BasicMaterial(glm::vec4 color) : Material::Material(color, 0) {}
BasicMaterial::BasicMaterial(float r, float g, float b, float a) : Material::Material(r, g, b, a, 0) {}
BasicMaterial::~BasicMaterial() {}



/* --- POINTLIGHT --- */



PointLight::PointLight(glm::vec3 position, double intensity, glm::vec3 color) : Light::Light(position, intensity, color) {}
PointLight::PointLight(float x, float y, float z, double intensity, glm::vec3 color) : Light::Light(glm::vec3(x, y, z), intensity, color) {}

PointLight::~PointLight() {}



/* --- AMBIENTLIGHT --- */




AmbientLight::AmbientLight(double intensity, glm::vec3 color) : Light::Light(glm::vec3(0), intensity, color, true) {}
AmbientLight::~AmbientLight() {}
