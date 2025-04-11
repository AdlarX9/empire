#ifndef OPENGL_ORG
#define OPENGL_ORG

#include "../lib/glad/glad.h"
#include <glm/glm.hpp>
#include <string>

class Shader {
  private:
	GLuint m_shaderProgram;

  public:
	Shader(const char* vertexPath, const char* fragmentPath);

	std::string loadShaderSource(const char* path) const;
	void        use() const;
	GLuint      getShaderProgram() const;

	void addUniform(const char* uniformName, glm::vec3 vector);
	void addUniform(const char* uniformName, glm::vec4 vector);
	void addUniform(const char* uniformName, glm::mat4 matrix);
	void addUniform(const char* uniformName, unsigned int value);
	void addUniform(const char* uniformName, int value);
	void addUniform(const char* uniformName, float value);
	void addUniform(const char* uniformName, glm::vec3* array, unsigned int size);
	void addUniform(const char* uniformName, glm::vec4* array, unsigned int size);
	void addUniform(const char* uniformName, unsigned int* array, unsigned int size);
	void addUniform(const char* uniformName, int* array, unsigned int size);
	void addUniform(const char* uniformName, float* array, unsigned int size);

	~Shader();
};

#endif