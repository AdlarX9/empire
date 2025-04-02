#define WIDTH 2400
#define HEIGHT 1500

#include "main.hpp"

#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "../maths/utils.hpp"

using namespace std;




/* --- GEOMETRY --- */



Geometry::Geometry(unsigned int vertexCount, sf::Vector3<double>* vertices, unsigned int* faces, sf::Vector3<double>* normalVectors)
    : m_vertexCount(vertexCount), m_vertices(vertices), m_faces(faces), m_normalVectors(normalVectors) {}

unsigned int          Geometry::vertexCount() const { return m_vertexCount; }
sf::Vector3<double>*& Geometry::getVertices() { return m_vertices; }
unsigned int*&        Geometry::getFaces() { return m_faces; }
sf::Vector3<double>*& Geometry::getNormalVectors() { return m_normalVectors; }

Geometry::~Geometry() {
	delete[] m_vertices;
	delete[] m_faces;
	delete[] m_normalVectors;
}



/* --- MATERIAL --- */



Material::Material(sf::Color color) : m_mainColor(color) {}

sf::Color& Material::getMainColor() { return m_mainColor; }

Material& Material::setMainColor(sf::Color color) {
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



Camera::Camera(sf::Vector3<double> position, unsigned int fov, sf::Vector3<double> direction)
    : m_position(position), m_defaultDirection(direction), m_fov(fov), m_rotation(UnitQuaternion()) {}

sf::Vector3<double>& Camera::getPosition() { return m_position; }
sf::Vector3<double>& Camera::getDefaultDirection() { return m_defaultDirection; }
UnitQuaternion&      Camera::getRotation() { return m_rotation; }
unsigned int         Camera::getFov() const { return m_fov; }

Camera& Camera::lookAt(sf::Vector3<double>& point) {
	sf::Vector3<double> direction = point - m_position;
	direction = direction.normalized();

	sf::Vector3<double> defaultDirection = m_defaultDirection;
	defaultDirection = defaultDirection.normalized();

	double dotProduct = defaultDirection.dot(direction);
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
	sf::Vector3<double> point(x, y, z);
	this->lookAt(point);
	return *this;
}

Camera& Camera::translate(double x, double y, double z) {
	m_position += sf::Vector3<double>(x, y, z);
	return *this;
}

Camera::~Camera() {}



/* --- RENDERER --- */



Renderer::Renderer(sf::RenderTexture& renderTexture, Camera& camera, Scene& scene)
    : m_renderTexture(renderTexture),
      m_camera(camera),
      m_scene(scene),
      m_shader(sf::Shader()),
      m_sprite(sf::Sprite(renderTexture.getTexture())) {
	if (!m_shader.loadFromFile("src/shaders/rasterization.vert", "src/shaders/mapping.frag")) {
		cerr << "failed to load test.frag" << endl;
		exit(EXIT_FAILURE);
	}
}

sf::Sprite& Renderer::getSprite() { return m_sprite; }

void Renderer::render() {
	sf::VertexArray vertices(sf::PrimitiveType::Triangles);
	for (Mesh* mesh : m_scene.getMeshes()) {
		Geometry& geometry = mesh->getGeometry();
		Material& material = mesh->getMaterial();

		for (unsigned int i = 0; i < 36; i++) {
			sf::Vector3<double> vertexPos = geometry.getVertices()[geometry.getFaces()[i]];
			sf::Vertex vertex = {sf::Vector2f(vertexPos.x, vertexPos.y), material.getMainColor(), {static_cast<float>(vertexPos.z), 0.0f}};

			vertices.append(vertex);
			Quaternion rotatedVertex = m_camera.getRotation().getConjugate() *
			                           Quaternion(vertex.position.x, vertex.position.y, vertex.texCoords.x, 0) * m_camera.getRotation();
			rotatedVertex -= m_camera.getPosition();

			cout << endl;
			cout << vertex.position.x << " ; " << vertex.position.y << " ; " << vertex.texCoords.x << endl;
			cout << m_camera.getRotation().x() << " ; " << m_camera.getRotation().y() << " ; " << m_camera.getRotation().z() << " ; "
			     << m_camera.getRotation().w() << endl;
			cout << rotatedVertex.x() << " ; " << rotatedVertex.y() << " ; " << rotatedVertex.z() << endl;
		}
	}
	cout << endl << "----------------------------------------------------------------" << endl;

	m_shader.setUniform("cameraPosition", sf::Vector3f(m_camera.getPosition()));
	m_shader.setUniform("screenWidth", WIDTH);
	m_shader.setUniform("screenHeight", HEIGHT);

	UnitQuaternion cameraRotation = m_camera.getRotation();
	UnitQuaternion cameraRotationConjugate = m_camera.getRotation().getConjugate();

	sf::Glsl::Vec4 cameraRotationVec(static_cast<float>(cameraRotation.x()), static_cast<float>(cameraRotation.y()),
	                                 static_cast<float>(cameraRotation.z()), static_cast<float>(cameraRotation.w()));
	m_shader.setUniform("cameraRotation", cameraRotationVec);

	sf::Glsl::Vec4 cameraRotationConjugateVec(
	    static_cast<float>(cameraRotationConjugate.x()), static_cast<float>(cameraRotationConjugate.y()),
	    static_cast<float>(cameraRotationConjugate.z()), static_cast<float>(cameraRotationConjugate.w()));
	m_shader.setUniform("cameraRotationConjugate", cameraRotationConjugateVec);

	m_renderTexture.clear(sf::Color::Black);
	m_renderTexture.draw(vertices, &m_shader);
}

Renderer::~Renderer() {}



/* --- BOXGEOMETRY --- */



BoxGeometry::BoxGeometry(double x, double y, double z) : Geometry::Geometry(0, {}, {}, {}) {
	m_vertexCount = 8;

	double halfX = x / 2.0;
	double halfY = y / 2.0;
	double halfZ = z / 2.0;

	m_vertices = new sf::Vector3<double>[m_vertexCount];
	m_vertices[0] = {-halfX, -halfY, -halfZ};
	m_vertices[1] = {halfX, -halfY, -halfZ};
	m_vertices[2] = {halfX, halfY, -halfZ};
	m_vertices[3] = {-halfX, halfY, -halfZ};
	m_vertices[4] = {-halfX, halfY, halfZ};
	m_vertices[5] = {-halfX, -halfY, halfZ};
	m_vertices[6] = {halfX, -halfY, halfZ};
	m_vertices[7] = {halfX, halfY, halfZ};

	m_faces =
	    new unsigned int[36]{0, 1, 2, 0, 2, 3, 1, 2, 7, 1, 2, 6, 1, 0, 5, 1, 0, 6, 4, 5, 6, 4, 5, 7, 4, 3, 0, 4, 3, 5, 4, 7, 2, 4, 7, 3};

	m_normalVectors = new sf::Vector3<double>[12];
	m_normalVectors[0] = sf::Vector3<double>(0, 0, -1);
	m_normalVectors[1] = sf::Vector3<double>(0, 0, -1);
	m_normalVectors[2] = sf::Vector3<double>(1, 0, 0);
	m_normalVectors[3] = sf::Vector3<double>(1, 0, 0);
	m_normalVectors[4] = sf::Vector3<double>(0, -1, 0);
	m_normalVectors[5] = sf::Vector3<double>(0, -1, 0);
	m_normalVectors[6] = sf::Vector3<double>(0, 0, 1);
	m_normalVectors[7] = sf::Vector3<double>(0, 0, 1);
	m_normalVectors[8] = sf::Vector3<double>(-1, 0, 0);
	m_normalVectors[9] = sf::Vector3<double>(-1, 0, 0);
	m_normalVectors[10] = sf::Vector3<double>(0, 1, 0);
	m_normalVectors[11] = sf::Vector3<double>(0, 1, 0);
}

BoxGeometry::~BoxGeometry() {}
