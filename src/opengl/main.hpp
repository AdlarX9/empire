#ifndef OPENGL_ORG
#define OPENGL_ORG

#include "../lib/glad/glad.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>

class Shader {
  private:
	GLuint m_shaderProgram;

  public:
	Shader(const char* vertexPath, const char* fragmentPath);

	std::string loadShaderSource(const char* path) const;
	void        use();

	void addUniform(const char* uniformName, glm::vec3 const& vector);
	void addUniform(const char* uniformName, glm::vec4 const& vector);
	void addUniform(const char* uniformName, glm::mat4 const& matrix);
	void addUniform(const char* uniformName, unsigned int const& value);
	void addUniform(const char* uniformName, int const& value);
	void addUniform(const char* uniformName, float const& value);
	void addUniform(const char* uniformName, glm::vec3* const& array, unsigned int size);
	void addUniform(const char* uniformName, glm::vec4* const& array, unsigned int size);
	void addUniform(const char* uniformName, unsigned int* const& array, unsigned int size);
	void addUniform(const char* uniformName, int* const& array, unsigned int size);
	void addUniform(const char* uniformName, float* const& array, unsigned int size);

	~Shader();
};

class VAO {
  private:
	GLuint m_VAO;

  public:
	VAO();

	void bind();
	void unBind();

	~VAO();
};

class VBO {
  private:
	GLuint m_VBO;

  public:
	VBO();

	void bind(unsigned int size, GLfloat* data);
	void unBind();

	~VBO();
};

class EBO {
  private:
	GLuint m_EBO;
	std::vector<GLuint> m_attrIndices;

  public:
	EBO();

	void bind(unsigned int size, GLuint* data);
	void addAttribute(GLuint index, GLuint size, GLuint offset, GLuint stride, GLenum type = GL_FLOAT, GLboolean normalized = GL_FALSE);
	void unBind();

	~EBO();
};

#endif