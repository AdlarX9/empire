#ifndef THREE_MAIN
#define THREE_MAIN

#include "../maths/utils.hpp"
#include "../opengl/main.hpp"
#include "../lib/glad/glad.h"

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <cmath>

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
	float     m_metalness;

  public:
	Material(glm::vec4 color = glm::vec4(1, 1, 1, 1), float metalness = 1);
	Material(float r, float g, float b, float a = 0.0f, float metalness = 1);

	glm::vec4&           getMainColor();
	Material&            setMainColor(glm::vec4 color);
	float                getMetalness() const;
	Material&            setMetalness(float metalness);
	virtual void         finalRender(unsigned int faceCount) const;
	virtual GLuint*      getFacesData(unsigned int faceCount) const;
	virtual unsigned int alterFaceCount(unsigned int faceCount) const;

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
	Mesh&           setRotation(UnitQuaternion& rotation);
	glm::vec3&      getTranslation();
	Mesh&           setTranslation(glm::vec3& translation);
	glm::vec3&      getScale();
	GLfloat*        getVerticesData() const;
	unsigned int    faceCount() const;
	Mesh&           translate(glm::vec3 translation);
	Mesh&           translate(float dx, float dy, float dz);
	Mesh&           rotateSelf(UnitQuaternion rotation, glm::vec3 point = glm::vec3(0));
	Mesh&           rotateSelf(float angle, glm::vec3(axis), glm::vec3 point = glm::vec3(0));
	Mesh&           rotateScene(UnitQuaternion rotation);
	Mesh&           rotateScene(float angle, glm::vec3(axis));
	glm::vec3       transform(glm::vec3 point) const;
	glm::vec3       invertTransform(glm::vec3 point) const;

	~Mesh();
};

class Light {
  protected:
	glm::vec3 m_position;
	double    m_intensity;
	glm::vec3 m_color;
	bool      m_ambient;

  public:
	Light(glm::vec3 position, double intensity, glm::vec3 color = glm::vec3(1, 1, 1), bool ambient = false);

	glm::vec3& getPosition();
	float      getIntensity() const;
	glm::vec3& getColor();
	bool       getAmbient() const;

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

class PyramidGeometry : public Geometry {
  public:
	PyramidGeometry(float x = 1, float y = 1, float h = std::sqrt(0.5));
	~PyramidGeometry();
};

class TetrahedronGeometry : public Geometry {
  public:
	TetrahedronGeometry();
	~TetrahedronGeometry();
};

class SphereGeometry : public Geometry {
  public:
	SphereGeometry(float radius = 1, unsigned int verticals = 20, unsigned int rows = 20);
	~SphereGeometry();
};


// Materials

class BasicMaterial : public Material {
  public:
	BasicMaterial(glm::vec4 color = glm::vec4(1));
	BasicMaterial(float r, float g, float b, float a = 1);
	~BasicMaterial();
};

class LinesMaterial : public Material {
  public:
	LinesMaterial(glm::vec4 color = glm::vec4(1), float metalness = 1);
	LinesMaterial(float r, float g, float b, float metalness = 1);

	void         finalRender(unsigned int faceCount) const;
	GLuint*      getFacesData(unsigned int faceCount) const;
	unsigned int alterFaceCount(unsigned int faceCount) const;

	~LinesMaterial();
};

class LinesBasicMaterial : public LinesMaterial {
  public:
	LinesBasicMaterial(glm::vec4 color = glm::vec4(1));
	LinesBasicMaterial(float r, float g, float b);
	~LinesBasicMaterial();
};

class PointsMaterial : public Material {
  public:
	PointsMaterial(glm::vec4 color = glm::vec4(1), float metalness = 1);
	PointsMaterial(float r, float g, float b, float metalness = 1);

	void finalRender(unsigned int faceCount) const;

	~PointsMaterial();
};

class PointsBasicMaterial : public PointsMaterial {
  public:
	PointsBasicMaterial(glm::vec4 color = glm::vec4(1));
	PointsBasicMaterial(float r, float g, float b);
	~PointsBasicMaterial();
};


// Lumières

class PointLight : public Light {
  public:
	PointLight(glm::vec3 position = glm::vec3(0, 0, 0), double intensity = 1, glm::vec3 color = glm::vec3(1, 1, 1));
	PointLight(float x, float y, float z, double intensity = 1, glm::vec3 color = glm::vec3(1, 1, 1));
	~PointLight();
};

class AmbientLight : public Light {
  public:
	AmbientLight(double intensity = 1, glm::vec3 color = glm::vec3(1, 1, 1));
	~AmbientLight();
};

#endif
