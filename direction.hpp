#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "light.hpp"
class directionlight : public light

{
public:
	glm::vec3 direction;

	directionlight(glm::vec3 direction) : light()
	{
		this->direction = direction;
	}

	void setdirection(GLuint shaderProg)
	{
		GLuint directionAddress = glGetUniformLocation(shaderProg, "direction");
		glUniform3fv(directionAddress, 1, glm::value_ptr(this->direction));
		GLuint colorAddress = glGetUniformLocation(shaderProg, "dirlightcolor");
		glUniform3fv(colorAddress, 1, glm::value_ptr(this->lightColor));
		
	}

};