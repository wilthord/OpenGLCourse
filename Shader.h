#pragma once

#include <stdio.h>
#include <String>
#include <iostream>
#include <fstream>

#include <GL/glew.h>

class Shader
{
public: 
	Shader();

	void createFromString(const char* vertexCode, const char* fragmentCode);
	void createFromFile(const char* vertexCode, const char* fragmentCode);

	std::string readFile(const char* fileLocation);

	GLuint getProjectionLocation();
	GLuint getModelLocation();
	GLuint getViewLocation();
	GLuint getAmbientIntensityLocation();
	GLuint getAmbientColourLocation();
	GLuint getDiffuseIntensityLocation();
	GLuint getDirectionLocation();
	GLuint getEyePositionLocation();
	GLuint getSpecularIntensityLocation();
	GLuint getShininessLocation();

	void useShader();
	void clearShader();

	~Shader();

private:
	GLuint shaderId, uniformProjection, uniformModel, uniformView, uniformEyePosition;
	GLuint uniformAmbientColour, uniformAmbientIntensity;
	GLuint uniformDiffuseIntensity, uniformDirection;
	GLuint uniformSpecularIntensity, uniformShininess;

	void compileShader(const char* vertexCode, const char* fragmentCode);
	void addShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);
};

