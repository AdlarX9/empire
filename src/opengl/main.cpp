#include "main.hpp"

#include "../lib/glad/glad.h"
#include <glm/glm.hpp>
#include <string>
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

GLuint Shader::getShaderProgram() const { return m_shaderProgram; }

Shader::~Shader() { glDeleteProgram(m_shaderProgram); }
