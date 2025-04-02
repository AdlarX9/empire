#ifndef THREE_MAIN
#define THREE_MAIN

#include <SFML/Graphics.hpp>
#include <vector>

#include "../maths/utils.hpp"

class Geometry {
  protected:
	unsigned int         m_vertexCount;
	sf::Vector3<double>* m_vertices;
	unsigned int*        m_faces;
	sf::Vector3<double>* m_normalVectors;

  public:
	Geometry(unsigned int vertexCount, sf::Vector3<double>* vertices, unsigned int* faces, sf::Vector3<double>* normalVectors);

	unsigned int          vertexCount() const;
	sf::Vector3<double>*& getVertices();
	unsigned int*&        getFaces();
	sf::Vector3<double>*& getNormalVectors();

	~Geometry();
};



class Material {
  protected:
	sf::Color m_mainColor;

  public:
	Material(sf::Color color = sf::Color::White);
	sf::Color& getMainColor();
	Material&  setMainColor(sf::Color color);
	~Material();
};


class Mesh {
  protected:
	Geometry& m_geometry;
	Material& m_material;

  public:
	Mesh(Geometry& geometry, Material& material);

	Geometry& getGeometry();
	Material& getMaterial();

	~Mesh();
};



class Scene {
  protected:
	std::vector<Mesh*> m_meshes;

  public:
	Scene();

	std::vector<Mesh*>& getMeshes();
	Scene&              add(Mesh* mesh);
	Scene&              remove(Mesh* mesh);

	~Scene();
};



class Camera {
  protected:
	sf::Vector3<double> m_position;
	sf::Vector3<double> m_defaultDirection;
	UnitQuaternion      m_rotation;
	unsigned int        m_fov;

  public:
	Camera(sf::Vector3<double> position, unsigned int fov = 60, sf::Vector3<double> direction = sf::Vector3<double>(0, 0, 1));

	sf::Vector3<double>& getPosition();
	sf::Vector3<double>& getDefaultDirection();
	UnitQuaternion&      getRotation();
	unsigned int         getFov() const;
	Camera&              lookAt(sf::Vector3<double>& point);
	Camera&              lookAt(double x, double y, double z);
	Camera&              translate(double x = 0, double y = 0, double z = 0);

	~Camera();
};



class Renderer {
  protected:
	sf::RenderTexture& m_renderTexture;
	sf::Sprite         m_sprite;
	Camera&            m_camera;
	Scene&             m_scene;
	sf::Shader         m_shader;

  public:
	Renderer(sf::RenderTexture& m_renderTexture, Camera& camera, Scene& scene);

	void        render();
	sf::Sprite& getSprite();

	~Renderer();
};


class BoxGeometry : public Geometry {
  public:
	BoxGeometry(double width = 1, double length = 1, double height = 1);
	~BoxGeometry();
};

#endif
