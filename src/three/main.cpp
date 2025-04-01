#include "main.hpp"

#include <SFML/Graphics.hpp>
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



Camera::Camera(sf::Vector3<double> position, sf::Vector3<double> direction, unsigned int m_fov)
    : m_position(position), m_direction(direction), m_fov(m_fov) {}

sf::Vector3<double>& Camera::getPosition() { return m_position; }
sf::Vector3<double>& Camera::getDirection() { return m_direction; }
unsigned int         Camera::getFov() const { return m_fov; }

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
	m_renderTexture.clear(sf::Color::Black);
	m_renderTexture.draw(m_sprite, &m_shader);
}

Renderer::~Renderer() {}
