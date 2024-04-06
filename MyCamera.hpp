#pragma once

class MyCamera {
protected:
	glm::vec3 cameraPos;
	glm::vec3 viewFront;
	glm::vec3 WorldUp;

	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;

	bool camera1;
	bool camera2;

public:
	MyCamera() {
		this->cameraPos = {};
		this->viewFront = {};
		this->WorldUp = glm::vec3(0, 1.0f, 0);

		this->viewMatrix = {};
		this->projectionMatrix = {};

		this->camera1 = true;
		this->camera2 = false;
	}

	void projection_viewMatrix(GLuint shaderProg) {

		unsigned int projectionLoc = glGetUniformLocation(shaderProg, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(this->projectionMatrix));

		unsigned int viewLoc = glGetUniformLocation(shaderProg, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(this->viewMatrix));
	}

	glm::vec3 getCameraPos() {
		return this->cameraPos;
	}

	glm::mat4 getViewMatrix() {
		return this->viewMatrix;
	}

	glm::mat4 getProjectionMatrix() {
		return this->projectionMatrix;
	}

	void setViewMatrix(glm::vec3 viewFront) {
		this->viewMatrix = glm::lookAt(this->cameraPos, viewFront, this->WorldUp);
	}

	virtual void setCameraPos(glm::vec3 cameraPos) = 0;

	virtual void setCamera(bool set) = 0;

	virtual bool getCamera() = 0;
};

class PerspectiveCamera : public MyCamera {
public:
	PerspectiveCamera() : MyCamera() {

	}

	void setCameraPos(glm::vec3 cameraPos) {
		this->cameraPos = cameraPos;
	}

	void setProjectionMatrix(float window_width, float window_height, int type) {
		if (type == 0) {
			this->projectionMatrix = glm::perspective(
				glm::radians(60.f),//FOV
				window_width / window_height, //aspect ratio
				0.1f, //znear > 0
				100.f //zfar
			);
		}
		if (type == 1) {
			this->projectionMatrix = glm::perspective(
				glm::radians(60.f),//FOV
				window_width / window_height, //aspect ratio
				0.1f, //znear > 0
				40.f //zfar
			);
		}
	}

	void setCamera(bool set) {
		this->camera1 = set;
	}

	bool getCamera() {
		return this->camera1;
	}

};

class OrthographicCamera : public MyCamera {
public:
	OrthographicCamera() : MyCamera() {

	}

	void setCameraPos(glm::vec3 cameraPos) {
		this->cameraPos = cameraPos;
	}

	void setProjectionMatrix() {
		this->projectionMatrix = glm::ortho(
			-10.0f,  // left
			10.0f,   // right
			-10.0f,  // bottom
			10.0f,   // top
			-100.0f, // near
			100.0f //far
		);
	}



	void setCamera(bool set) {
		this->camera2 = set;
	}

	bool getCamera() {
		return this->camera2;
	}
};

