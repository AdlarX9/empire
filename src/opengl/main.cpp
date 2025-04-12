#include "main.hpp"

#include "../lib/glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;



/* --- SHADER --- */



Shader::Shader(const char* vertexPath, const char* fragmentPath) {
	// Charger les shaders
	string vertexString = loadShaderSource(vertexPath);
	string fragmentString = loadShaderSource(fragmentPath);

	const char* vertexShaderSource = vertexString.c_str();
	const char* fragmentShaderSource = fragmentString.c_str();

	// Créer et compiler le vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);

	// Vérifier la compilation du vertex shader
	GLint success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[1024];  // Plus grand tampon pour capturer des logs plus détaillés
		glGetShaderInfoLog(vertexShader, 1024, nullptr, infoLog);
		cerr << "Erreur de compilation du vertex shader :\n" << infoLog << endl;
	}

	// Créer et compiler le fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);

	// Vérifier la compilation du fragment shader
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[1024];
		glGetShaderInfoLog(fragmentShader, 1024, nullptr, infoLog);
		cerr << "Erreur de compilation du fragment shader :\n" << infoLog << endl;
	}

	m_shaderProgram = glCreateProgram();

	glAttachShader(m_shaderProgram, vertexShader);
	glAttachShader(m_shaderProgram, fragmentShader);

	// Lier les shaders ensemble dans le programme
	glLinkProgram(m_shaderProgram);

	// Vérifier le linkage du programme
	glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[1024];
		glGetProgramInfoLog(m_shaderProgram, 1024, nullptr, infoLog);
		cerr << "Erreur de linkage du shader program :\n" << infoLog << endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}


void Shader::use() { glUseProgram(m_shaderProgram); }

string Shader::loadShaderSource(const char* path) const {
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

void Shader::addUniform(const char* uniformName, glm::vec3 const& vector) {
	GLuint uniformLoc = glGetUniformLocation(m_shaderProgram, uniformName);
	glUniform3f(uniformLoc, vector.x, vector.y, vector.z);
}

void Shader::addUniform(const char* uniformName, glm::vec4 const& vector) {
	GLuint uniformLoc = glGetUniformLocation(m_shaderProgram, uniformName);
	glUniform4f(uniformLoc, vector.x, vector.y, vector.z, vector.w);
}

void Shader::addUniform(const char* uniformName, glm::mat4 const& matrix) {
	GLuint uniformLoc = glGetUniformLocation(m_shaderProgram, uniformName);
	glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::addUniform(const char* uniformName, unsigned int const& value) {
	GLuint uniformLoc = glGetUniformLocation(m_shaderProgram, uniformName);
	glUniform1ui(uniformLoc, value);
}

void Shader::addUniform(const char* uniformName, int const& value) {
	GLuint uniformLoc = glGetUniformLocation(m_shaderProgram, uniformName);
	glUniform1i(uniformLoc, value);
}

void Shader::addUniform(const char* uniformName, float const& value) {
	GLuint uniformLoc = glGetUniformLocation(m_shaderProgram, uniformName);
	glUniform1f(uniformLoc, value);
}

void Shader::addUniform(const char* uniformName, glm::vec3* const& array, unsigned int size) {
	GLuint uniformLoc = glGetUniformLocation(m_shaderProgram, uniformName);
	glUniform3fv(uniformLoc, size, glm::value_ptr(array[0]));
}

void Shader::addUniform(const char* uniformName, glm::vec4* const& array, unsigned int size) {
	GLuint uniformLoc = glGetUniformLocation(m_shaderProgram, uniformName);
	glUniform4fv(uniformLoc, size, glm::value_ptr(array[0]));
}

void Shader::addUniform(const char* uniformName, unsigned int* const& array, unsigned int size) {
	GLuint uniformLoc = glGetUniformLocation(m_shaderProgram, uniformName);
	glUniform1uiv(uniformLoc, size, array);
}

void Shader::addUniform(const char* uniformName, int* const& array, unsigned int size) {
	GLuint uniformLoc = glGetUniformLocation(m_shaderProgram, uniformName);
	glUniform1iv(uniformLoc, size, array);
}

void Shader::addUniform(const char* uniformName, float* const& array, unsigned int size) {
	GLuint uniformLoc = glGetUniformLocation(m_shaderProgram, uniformName);
	glUniform1fv(uniformLoc, size, array);
}

Shader::~Shader() { glDeleteProgram(m_shaderProgram); }



/* --- VAO --- */



VAO::VAO() : m_VAO() { glGenVertexArrays(1, &m_VAO); }

void VAO::bind() { glBindVertexArray(m_VAO); }
void VAO::unBind() { glBindVertexArray(0); }

VAO::~VAO() { glDeleteVertexArrays(1, &m_VAO); }



/* --- VAO --- */



VBO::VBO() : m_VBO() { glGenBuffers(1, &m_VBO); }

void VBO::bind(unsigned int size, GLfloat* data) {
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(GLfloat), data, GL_STATIC_DRAW);
}

void VBO::unBind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }

VBO::~VBO() { glDeleteBuffers(1, &m_VBO); }



/* --- VAO --- */




EBO::EBO() : m_EBO(), m_attrIndices(vector<GLuint>()) { glGenBuffers(1, &m_EBO); }

void EBO::bind(unsigned int size, GLuint* data) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(GLuint), data, GL_STATIC_DRAW);
}

void EBO::addAttribute(GLuint index, GLuint size, GLuint offset, GLuint stride, GLenum type, GLboolean normalized) {
	size_t typeSize;
	if (type == GL_FLOAT) {
		typeSize = sizeof(GLfloat);
	} else if (type == GL_INT) {
		typeSize = sizeof(GLint);
	} else if (type == GL_UNSIGNED_INT) {
		typeSize = sizeof(GLuint);
	}

	m_attrIndices.push_back(index);
	glVertexAttribPointer(index, size, type, normalized, stride * typeSize, (GLvoid*)(offset * typeSize));
	glEnableVertexAttribArray(index);
}

void EBO::unBind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	for (GLuint index: m_attrIndices) {
		glDisableVertexAttribArray(index);
	}
}

EBO::~EBO() { glDeleteBuffers(1, &m_EBO); }
