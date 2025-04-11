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



Material::Material(glm::vec4 color) : m_mainColor(color) {}
Material::Material(float r, float g, float b, float a) : m_mainColor(glm::vec4(r, g, b, a)) {}

glm::vec4& Material::getMainColor() { return m_mainColor; }

Material& Material::setMainColor(glm::vec4 color) {
	m_mainColor = color;
	return *this;
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
	// Calculer la taille totale en fonction du nombre de faces et de la structure des données
	GLfloat* data = new GLfloat[m_geometry.faceCount() * 9 * nbrVectors];  // 6 éléments par vertex : 3 pour la position, 3 pour la normale

	GLfloat* vertices = m_geometry.getVertices();
	GLfloat* normalVectors = m_geometry.getNormalVectors();
	GLuint*  faces = m_geometry.getFaces();

	unsigned int index = 0;

	for (unsigned int i = 0; i < m_geometry.faceCount(); i++) {
		// Pour chaque face, tu as 3 vertices
		for (unsigned int j = 0; j < 3; j++) {
			unsigned int vertexIndex = faces[i * 3 + j];      // Récupérer l'indice du vertex
			data[index] = vertices[vertexIndex * 3];          // x
			data[index + 1] = vertices[vertexIndex * 3 + 1];  // y
			data[index + 2] = vertices[vertexIndex * 3 + 2];  // z

			// Assumer qu'il y a une normale par face
			unsigned int normalIndex = i;                          // Si tu as une normale par face
			data[index + 3] = normalVectors[normalIndex * 3];      // nx
			data[index + 4] = normalVectors[normalIndex * 3 + 1];  // ny
			data[index + 5] = normalVectors[normalIndex * 3 + 2];  // nz

			index += 6;  // 6 éléments par vertex (position + normale)
		}
	}

	return data;
}

GLuint* Mesh::getFacesData() const {
	GLuint* facesData = new GLuint[m_geometry.faceCount() * 3];

	for (unsigned int i = 0; i < m_geometry.faceCount() * 3; i++) {
		facesData[i] = i;
	}

	return facesData;
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

LightStruct* Scene::getLightStructs() const {
	LightStruct* lights = new LightStruct[MAX_LIGHT];

	for (unsigned int i = 0; i < m_lights.size(); i++) {
		lights[i] = m_lights[i]->getLight();
	}

	for (unsigned int i = m_lights.size(); i < MAX_LIGHT; i++) {
		LightStruct light;
		light.position = glm::vec3(0, 0, 0);
		light.intensity = 0;
		light.color = glm::vec3(1, 1, 1);
		light.ambient = false;
		lights[i] = light;
	}

	return lights;
}

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
	m_shaderProgram = m_shader.getShaderProgram();
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
	glUseProgram(m_shaderProgram);

	// Envoyer la position de la caméra
	glm::vec3 cameraPos = m_camera.getPosition();
	GLuint    cameraPosLoc = glGetUniformLocation(m_shaderProgram, "cameraPos");
	glUniform3f(cameraPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);

	// Définir les matrices de vue et de projection
	glm::mat4 view = glm::lookAt(m_camera.getPosition(), m_camera.getPosition() + m_camera.getDirection(), glm::vec3(0, 0, 1));
	glm::mat4 projection = glm::perspective((float)glm::radians((float)m_camera.getFov()), (float)(WIDTH / HEIGHT), 0.1f, 100.0f);

	// Envoyer les matrices de vue et de projection
	GLuint viewLoc = glGetUniformLocation(m_shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	GLuint projectionLoc = glGetUniformLocation(m_shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// Envoyer les dimensions de l'écran
	GLuint screenWidthLoc = glGetUniformLocation(m_shaderProgram, "screenWidth");
	glUniform1i(screenWidthLoc, WIDTH);
	GLuint screenHeightLoc = glGetUniformLocation(m_shaderProgram, "screenHeight");
	glUniform1i(screenHeightLoc, HEIGHT);

	// Envoyer les lumières
	GLuint nbrLightsLoc = glGetUniformLocation(m_shaderProgram, "nbrLights");
	glUniform1ui(nbrLightsLoc, m_scene.getNbrLights());

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

	GLuint lightPositionsLoc = glGetUniformLocation(m_shaderProgram, "lightPositions");
	glUniform3fv(lightPositionsLoc, MAX_LIGHT, glm::value_ptr(positions[0]));
	GLuint lightIntensitiesLoc = glGetUniformLocation(m_shaderProgram, "lightIntensities");
	glUniform1fv(lightIntensitiesLoc, MAX_LIGHT, intensities);
	GLuint lightColorsLoc = glGetUniformLocation(m_shaderProgram, "lightColors");
	glUniform3fv(lightColorsLoc, MAX_LIGHT, glm::value_ptr(colors[0]));
	GLuint lightAmbientsLoc = glGetUniformLocation(m_shaderProgram, "lightAmbients");
	glUniform1fv(lightAmbientsLoc, MAX_LIGHT, ambients);


	// Nettoyer l'écran avec la couleur de fond
	glm::vec3 backgroundColor = m_scene.getBackGroundColor();
	glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (Mesh* mesh : m_scene.getMeshes()) {
		Geometry& geometry = mesh->getGeometry();
		Material& material = mesh->getMaterial();

		// Récupération des données
		GLfloat* verticesData = mesh->getVerticesData();
		GLuint*  facesData = mesh->getFacesData();

		glm::vec4 color = material.getMainColor();

		// Création des objets OpenGL
		GLuint VAO, VBO, EBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		size_t faceCount = geometry.faceCount();

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, faceCount * 3 * 6 * sizeof(GLfloat), verticesData, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceCount * 3 * sizeof(GLuint), facesData, GL_STATIC_DRAW);

		// Attribut 0 : Positions des vertices
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);

		// Attribut 1 : Normales
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		// Envoi de la matrice Model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, mesh->getTranslation());
		model = glm::rotate(model, mesh->getRotation().getAngle(), mesh->getRotation().getAxis());
		model = glm::scale(model, mesh->getScale());
		GLuint modelLoc = glGetUniformLocation(m_shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		// Envoi de la couleur de l'objet
		glm::vec4 objectColor = material.getMainColor();
		GLuint    objectColorLoc = glGetUniformLocation(m_shaderProgram, "objectColor");
		glUniform4f(objectColorLoc, objectColor.r, objectColor.g, objectColor.b, objectColor.a);

		// Rendu
		glDrawElements(GL_TRIANGLES, geometry.faceCount() * 3, GL_UNSIGNED_INT, 0);

		// Supprimer les données allouées
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		delete[] verticesData;
		delete[] facesData;
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



/* --- POINTLIGHT --- */



PointLight::PointLight(glm::vec3 position, double intensity, glm::vec3 color) : Light::Light(position, intensity, color) {}
PointLight::PointLight(float x, float y, float z, double intensity, glm::vec3 color) : Light::Light(glm::vec3(x, y, z), intensity, color) {}

struct LightStruct PointLight::getLight() {
	struct LightStruct light;
	light.position = m_position;
	light.intensity = m_intensity;
	light.color = m_color;
	light.ambient = m_ambient;
	return light;
}

PointLight::~PointLight() {}
