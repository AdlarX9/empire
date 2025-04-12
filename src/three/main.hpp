#ifndef THREE_MAIN
#define THREE_MAIN

#include "../maths/utils.hpp"
#include "../opengl/main.hpp"
#include "../lib/glad/glad.h"

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>

class Geometry {
  protected:
	unsigned int m_vertexCount;
	GLfloat*     m_vertices;
	unsigned int m_faceCount;
	GLuint*      m_faces;
	GLfloat*     m_normalVectors;

  public:
	Geometry(unsigned int vertexCount, GLfloat* vertices, unsigned int faceCount, GLuint* faces, GLfloat* normalVectors);

	unsigned int vertexCount() const;
	GLfloat*     getVertices() const;
	unsigned int faceCount() const;
	GLuint*      getFaces() const;
	GLfloat*&    getNormalVectors();

	~Geometry();
};



class Material {
  protected:
	glm::vec4 m_mainColor;

  public:
	Material(glm::vec4 color = glm::vec4(1, 1, 1, 1));
	Material(float r, float g, float b, float a = 0.0f);
	glm::vec4& getMainColor();
	Material&  setMainColor(glm::vec4 color);
	~Material();
};


class Mesh {
  protected:
	Geometry&      m_geometry;
	Material&      m_material;
	UnitQuaternion m_rotation;
	glm::vec3      m_translation;
	glm::vec3      m_scale;

  public:
	Mesh(Geometry& geometry, Material& material);

	Geometry&       getGeometry();
	Material&       getMaterial();
	UnitQuaternion& getRotation();
	glm::vec3&      getTranslation();
	glm::vec3&      getScale();
	GLfloat*        getVerticesData() const;
	GLuint*         getFacesData() const;

	~Mesh();
};


struct LightStruct {
	glm::vec3 position;
	double    intensity;
	glm::vec3 color;
	bool      ambient;
};

class Light {
  protected:
	glm::vec3 m_position;
	double    m_intensity;
	glm::vec3 m_color;
	bool      m_ambient;

  public:
	Light(glm::vec3 position, double intensity, glm::vec3 color = glm::vec3(1, 1, 1), bool ambient = false);

	virtual struct LightStruct getLight() = 0;
	glm::vec3&                 getPosition();
	float                      getIntensity() const;
	glm::vec3&                 getColor();
	bool                       getAmbient() const;

	~Light();
};



class Scene {
  private:
	std::vector<Mesh*>  m_meshes;
	std::vector<Light*> m_lights;
	glm::vec3           m_backgroundColor;

  public:
	Scene(glm::vec3 backgroundColor = glm::vec3(0.07f, 0.13f, 0.17f));
	Scene(float r, float g, float b);

	std::vector<Mesh*>&  getMeshes();
	glm::vec3&           getBackGroundColor();
	unsigned int         getNbrLights() const;
	std::vector<Light*>& getLights();
	LightStruct*         getLightStructs() const;
	Scene&               setBackGroundColor(glm::vec3 backgroundColor);
	Scene&               setBackGroundColor(float r, float g, float b);
	Scene&               add(Mesh* mesh);
	Scene&               remove(Mesh* mesh);
	Scene&               add(Light* light);
	Scene&               remove(Light* light);

	~Scene();
};



class Camera {
  protected:
	glm::vec3    m_position;
	glm::vec3    m_defaultDirection;
	glm::vec3    m_direction;
	unsigned int m_fov;
	glm::vec2    m_mousePos;
	GLFWwindow*  m_window;

  public:
	Camera(GLFWwindow* window, glm::vec3 position, unsigned int fov = 90, glm::vec3 direction = glm::vec3(0, 0, 1));

	glm::vec3&   getPosition();
	glm::vec3&   getDefaultDirection();
	glm::vec3&   getDirection();
	glm::vec3    getXAxis() const;
	glm::vec3    getYAxis() const;
	unsigned int getFov() const;
	Camera&      lookAt(const glm::vec3& point);
	Camera&      lookAt(double x, double y, double z);
	Camera&      translate(double x = 0, double y = 0, double z = 0);
	void         handleKeyControls(float deltaTime);
	void         handleMouseControls();

	~Camera();
};



class Renderer {
  protected:
	Camera& m_camera;
	Scene&  m_scene;
	Shader  m_shader;

  public:
	Renderer(Camera& camera, Scene& scene);

	void initializeUniforms();
	void clearScreen();
	void render();

	~Renderer();
};


// Géometries

class BoxGeometry : public Geometry {
  public:
	BoxGeometry(double width = 1, double length = 1, double height = 1);
	~BoxGeometry();
};

class PlaneGeometry : public Geometry {
  public:
	PlaneGeometry(double x, double y);
	~PlaneGeometry();
};


// Lumières

class PointLight : public Light {
  public:
	PointLight(glm::vec3 position = glm::vec3(0, 0, 0), double intensity = 1, glm::vec3 color = glm::vec3(1, 1, 1));
	PointLight(float x, float y, float z, double intensity = 1, glm::vec3 color = glm::vec3(1, 1, 1));

	struct LightStruct getLight();

	~PointLight();
};

#endif
