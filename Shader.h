#pragma once

#include <stdio.h>
#include <String>
#include <iostream>
#include <fstream>

#include <GL/glew.h>

#include "DirectionalLight.h"
#include "PointLight.h"
#include "CommonValues.h"

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

	void setDirectionalLight(DirectionalLight* dLight);
	void setPointLights(PointLight* pLight, unsigned int lightCount);
	
	void useShader();
	void clearShader();

	~Shader();

private:
	int pointLightCount;

	GLuint shaderId, uniformProjection, uniformModel, uniformView, uniformEyePosition;
	GLuint uniformSpecularIntensity, uniformShininess;
	GLuint uniformPointLightCount;

	struct {
		GLuint uniformColour;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformDirection;
	} uniformDirectionalLight;

	struct {
		GLuint uniformColour;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformPosition;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformExponent;
	} uniformPointLight[MAX_POINT_LIGHTS];

	void compileShader(const char* vertexCode, const char* fragmentCode);
	void addShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);
};

