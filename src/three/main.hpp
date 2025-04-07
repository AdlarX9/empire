#ifndef THREE_MAIN
#define THREE_MAIN

#include "../lib/glad/glad.h"
#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "../maths/utils.hpp"

class Geometry {
  protected:
	unsigned int m_vertexCount;
	GLfloat*     m_vertices;
	GLuint*      m_faces;
	GLfloat*     m_normalVectors;

  public:
	Geometry(unsigned int vertexCount, GLfloat* vertices, GLuint* faces, GLfloat* normalVectors);

	unsigned int vertexCount() const;
	GLfloat*&    getVertices();
	GLuint*&     getFaces();
	GLfloat*&    getNormalVectors();

	~Geometry();
};



class Material {
  protected:
	glm::vec4 m_mainColor;

  public:
	Material(glm::vec4 color = glm::vec4(1, 1, 1, 1));
	glm::vec4& getMainColor();
	Material&  setMainColor(glm::vec4 color);
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
	glm::vec3      m_position;
	glm::vec3      m_defaultDirection;
	UnitQuaternion m_rotation;
	unsigned int   m_fov;

  public:
	Camera(glm::vec3 position, unsigned int fov = 60, glm::vec3 direction = glm::vec3(0, 0, 1));

	glm::vec3&      getPosition();
	glm::vec3&      getDefaultDirection();
	UnitQuaternion& getRotation();
	unsigned int    getFov() const;
	Camera&         lookAt(glm::vec3& point);
	Camera&         lookAt(double x, double y, double z);
	Camera&         translate(double x = 0, double y = 0, double z = 0);

	~Camera();
};



class Renderer {
  protected:
	Camera& m_camera;
	Scene&  m_scene;
	GLuint  m_shaderProgram;

  public:
	Renderer(Camera& camera, Scene& scene);

	std::string loadShader(const char* path);
	void        render();

	~Renderer();
};


class BoxGeometry : public Geometry {
  public:
	BoxGeometry(double width = 1, double length = 1, double height = 1);
	~BoxGeometry();
};

#endif
