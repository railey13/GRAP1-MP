#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "light.hpp"

class pointlight : public light
{
public:
	float brightness;


	pointlight(float brightness) : light()
	{
		this->brightness = brightness;
	}

	void setbrightness(GLuint shaderProg)
	{
		GLuint brightnessAddress = glGetUniformLocation(shaderProg, "brightness");
		glUniform1f(brightnessAddress, (this->brightness));
	}
};