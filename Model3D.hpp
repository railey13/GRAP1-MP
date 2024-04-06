#pragma once

class Model3D {
protected:

	GLuint VAO, VBO;

	glm::vec3 modelPos;
	glm::vec3 modelScale;
	glm::vec3 modelTheta;
	glm::vec3 modelRX;
	glm::vec3 modelRY;
	glm::vec3 modelRZ;

	float modelScaleMod;

	int modelType;

	GLuint texture;

	std::vector<GLfloat> fullVertexData;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t>material;
	tinyobj::attrib_t attributes;
	glm::mat4 identity_matrix;
	glm::mat4 transformation_matrix;

	const char* image;

	const char* object;

public:
	Model3D(glm::vec3 pos, const char* image, const char* object, glm::vec3 scale, int i) {
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);
		glBindVertexArray(this->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

		this->modelPos = pos;
		this->modelScale = scale;
		this->modelScaleMod = 1.f;

		this->modelTheta = glm::vec3(0.f, 0.f, 0.f);
		this->modelRX = glm::vec3(1.f, 0.f, 0.f);
		this->modelRY = glm::vec3(0.0f, 1.f, 0.f);
		this->modelRZ = glm::vec3(0.f, 0.f, 1.f);

		this->identity_matrix = glm::mat4(1.0f);

		this->texture = NULL;
		this->transformation_matrix = {};

		this->image = image;
		this->object = object;

		this->modelType = i;
	}

public:
	void spawn() {
		this->loadTexture();

		this->loadObject();

		this->buildVertexData();

		this->loadVertexData();
	}

	void update(GLuint shaderProg) {
		this->transformationMatrix(shaderProg);
	}

	void draw() {
		glDrawArrays(GL_TRIANGLES, 0, this->fullVertexData.size() / 8);
	}

	void setPosition(glm::vec3 pos) {
		this->modelPos = pos;
	}

	void setYTheta(float y) {
		this->modelTheta.y = y;
	}

	glm::vec3 getPosition() {
		return this->modelPos;
	}

	glm::vec3 getThetas() {
		return this->modelTheta;
	}

	GLuint getVAO() {
		return this->VAO;
	}

	GLuint getTexture() {
		return this->texture;
	}

	int getModelType() {
		return this->modelType;
	}

private:
	virtual void loadTexture() {
		int img_width, img_height, colorChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* tex_bytes = stbi_load(this->image,
			&img_width,
			&img_height,
			&colorChannels,
			0);

		glGenTextures(1, &this->texture);

		glActiveTexture(GL_TEXTURE0);

		glBindTexture(GL_TEXTURE_2D, this->texture);

		glTexImage2D(GL_TEXTURE_2D,
			0,             //texture 0
			GL_RGBA,       //target color format of the texture // change to RGBA for png, RGB for jpg
			img_width,   //texture width
			img_height, //texture height
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			tex_bytes);       //loaded texture in bytes

		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(tex_bytes);
		glEnable(GL_DEPTH_TEST);
	}

	void loadObject() {
		std::string path = this->object;
		std::string warning, error;

		bool success = tinyobj::LoadObj(
			&this->attributes,
			&this->shapes,
			&this->material,
			&warning,
			&error,
			path.c_str()
		);
	}

	virtual void buildVertexData() {
		for (int i = 0; i < this->shapes[0].mesh.indices.size(); i++) {
			tinyobj::index_t vData = this->shapes[0].mesh.indices[i];

			this->fullVertexData.push_back(this->attributes.vertices[(vData.vertex_index * 3)]);
			this->fullVertexData.push_back(this->attributes.vertices[(vData.vertex_index * 3) + 1]);
			this->fullVertexData.push_back(this->attributes.vertices[(vData.vertex_index * 3) + 2]);

			this->fullVertexData.push_back(this->attributes.normals[(vData.normal_index * 3)]);
			this->fullVertexData.push_back(this->attributes.normals[(vData.normal_index * 3) + 1]);
			this->fullVertexData.push_back(this->attributes.normals[(vData.normal_index * 3) + 2]);

			this->fullVertexData.push_back(this->attributes.texcoords[(vData.texcoord_index * 2)]);
			this->fullVertexData.push_back(this->attributes.texcoords[(vData.texcoord_index * 2) + 1]);
		}

	}

	virtual void loadVertexData() {
		glBufferData(GL_ARRAY_BUFFER,
			sizeof(GLfloat) * this->fullVertexData.size(),
			this->fullVertexData.data(),
			GL_DYNAMIC_DRAW);

		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			8 * sizeof(float),
			(void*)0
		);

		glEnableVertexAttribArray(0);

		GLintptr normPtr = 3 * sizeof(float);
		glVertexAttribPointer(
			1,
			3,
			GL_FLOAT,
			GL_FALSE,
			8 * sizeof(float),
			(void*)normPtr
		);

		glEnableVertexAttribArray(1);

		GLintptr uvPtr = 6 * sizeof(float);
		glVertexAttribPointer(
			2,
			2,
			GL_FLOAT,
			GL_FALSE,
			8 * sizeof(float),
			(void*)uvPtr
		);

		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

private:
	void transformationMatrix(GLuint shaderProg) {

		this->transformation_matrix = glm::translate(this->identity_matrix,
			this->modelPos);

		this->transformation_matrix = glm::scale(this->transformation_matrix,
			this->modelScale);

		this->transformation_matrix = glm::rotate(this->transformation_matrix,
			glm::radians(this->modelTheta.y),
			this->modelRY);

		this->transformation_matrix = glm::rotate(this->transformation_matrix,
			glm::radians(this->modelTheta.x),
			this->modelRX);

		this->transformation_matrix = glm::rotate(this->transformation_matrix,
			glm::radians(this->modelTheta.z),
			this->modelRZ);

		glUseProgram(shaderProg);
		unsigned int transformLoc = glGetUniformLocation(shaderProg, "transform");

		glUniformMatrix4fv(transformLoc,
			1,
			GL_FALSE,
			glm::value_ptr(this->transformation_matrix));
	}
};

// use for normal mapping
class Player : public Model3D {
private:
	GLuint normTex;

public:
	Player(glm::vec3 pos, const char* image, const char* object, glm::vec3 scale) : Model3D(pos, image, object, scale, 0) {
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);
		glBindVertexArray(this->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

		this->modelPos = pos;
		this->modelScale = scale;
		this->modelScaleMod = 1.f;

		this->modelTheta = glm::vec3(0.f, 0.f, 0.f);
		this->modelRX = glm::vec3(1.f, 0.f, 0.f);
		this->modelRY = glm::vec3(0.0f, 1.f, 0.f);
		this->modelRZ = glm::vec3(0.f, 0.f, 1.f);

		this->identity_matrix = glm::mat4(1.0f);

		this->texture = NULL;
		this->normTex = NULL;
		this->transformation_matrix = {};

		this->image = image;
		this->object = object;
	}

private:
	void loadTexture() override {
		std::cout << "here" << std::endl;
		int img_width, img_height, colorChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* tex_bytes = stbi_load(this->image,
			&img_width,
			&img_height,
			&colorChannels,
			0);

		glGenTextures(1, &this->texture);

		glActiveTexture(GL_TEXTURE0);

		glBindTexture(GL_TEXTURE_2D, this->texture);

		glTexImage2D(GL_TEXTURE_2D,
			0,             //texture 0
			GL_RGBA,       //target color format of the texture // change to RGBA for png, RGB for jpg
			img_width,   //texture width
			img_height, //texture height
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			tex_bytes);       //loaded texture in bytes

		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(tex_bytes);
		glEnable(GL_DEPTH_TEST);

		int img_width2, img_height2, colorChannels2;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* normal_bytes = stbi_load("3D/NormalMap2.png",
			&img_width2,
			&img_height2,
			&colorChannels2,
			0);

		glGenTextures(1, &this->normTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, this->normTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		glTexImage2D(GL_TEXTURE_2D,
			0,             //texture 0
			GL_RGBA,       //target color format of the texture // change to RGBA for png, RGB for jpg
			img_width2,   //texture width
			img_height2, //texture height
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			normal_bytes);       //loaded texture in bytes

		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(normal_bytes);
	}

	void buildVertexData() override {
		std::cout << "here" << std::endl;
		std::vector<glm::vec3> tangents;
		std::vector<glm::vec3> bitangents;

		for (int i = 0; i < this->shapes[0].mesh.indices.size(); i += 3) {
			tinyobj::index_t vData1 = shapes[0].mesh.indices[i];
			tinyobj::index_t vData2 = shapes[0].mesh.indices[i + 1];
			tinyobj::index_t vData3 = shapes[0].mesh.indices[i + 2];

			glm::vec3 v1 = glm::vec3(attributes.vertices[vData1.vertex_index * 3],
				attributes.vertices[(vData1.vertex_index * 3) + 1],
				attributes.vertices[(vData1.vertex_index * 3) + 2]);

			glm::vec3 v2 = glm::vec3(attributes.vertices[vData2.vertex_index * 3],
				attributes.vertices[(vData2.vertex_index * 3) + 1],
				attributes.vertices[(vData2.vertex_index * 3) + 2]);

			glm::vec3 v3 = glm::vec3(attributes.vertices[vData3.vertex_index * 3],
				attributes.vertices[(vData3.vertex_index * 3) + 1],
				attributes.vertices[(vData3.vertex_index * 3) + 2]);

			glm::vec2 uv1 = glm::vec2(attributes.texcoords[(vData1.texcoord_index * 2)],
				attributes.texcoords[(vData1.texcoord_index * 2) + 1]);

			glm::vec2 uv2 = glm::vec2(attributes.texcoords[(vData2.texcoord_index * 2)],
				attributes.texcoords[(vData2.texcoord_index * 2) + 1]);

			glm::vec2 uv3 = glm::vec2(attributes.texcoords[(vData3.texcoord_index * 2)],
				attributes.texcoords[(vData3.texcoord_index * 2) + 1]);

			glm::vec3 deltaPos1 = v2 - v1;
			glm::vec3 deltaPos2 = v3 - v1;

			glm::vec2 deltaUV1 = uv2 - uv1;
			glm::vec2 deltaUV2 = uv3 - uv1;

			float r = 1.0f / ((deltaUV1.x * deltaUV2.y) - deltaUV1.y * deltaUV2.x);

			glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;

			glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

			tangents.push_back(tangent);
			tangents.push_back(tangent);
			tangents.push_back(tangent);

			bitangents.push_back(bitangent);
			bitangents.push_back(bitangent);
			bitangents.push_back(bitangent);
		}

		for (int i = 0; i < this->shapes[0].mesh.indices.size(); i++) {
			tinyobj::index_t vData = this->shapes[0].mesh.indices[i];

			this->fullVertexData.push_back(this->attributes.vertices[(vData.vertex_index * 3)]);
			this->fullVertexData.push_back(this->attributes.vertices[(vData.vertex_index * 3) + 1]);
			this->fullVertexData.push_back(this->attributes.vertices[(vData.vertex_index * 3) + 2]);

			this->fullVertexData.push_back(this->attributes.normals[(vData.normal_index * 3)]);
			this->fullVertexData.push_back(this->attributes.normals[(vData.normal_index * 3) + 1]);
			this->fullVertexData.push_back(this->attributes.normals[(vData.normal_index * 3) + 2]);

			this->fullVertexData.push_back(this->attributes.texcoords[(vData.texcoord_index * 2)]);
			this->fullVertexData.push_back(this->attributes.texcoords[(vData.texcoord_index * 2) + 1]);

			this->fullVertexData.push_back(tangents[i].x);
			this->fullVertexData.push_back(tangents[i].y);
			this->fullVertexData.push_back(tangents[i].z);

			this->fullVertexData.push_back(bitangents[i].x);
			this->fullVertexData.push_back(bitangents[i].y);
			this->fullVertexData.push_back(bitangents[i].z);
		}

	}

	void loadVertexData() override {
		std::cout << "here" << std::endl;
		glBufferData(GL_ARRAY_BUFFER,
			sizeof(GLfloat) * this->fullVertexData.size(),
			this->fullVertexData.data(),
			GL_DYNAMIC_DRAW);

		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			14 * sizeof(GLfloat),
			(void*)0
		);

		glEnableVertexAttribArray(0);

		GLintptr normPtr = 3 * sizeof(float);
		glVertexAttribPointer(
			1,
			3,
			GL_FLOAT,
			GL_FALSE,
			14 * sizeof(GLfloat),
			(void*)normPtr
		);

		glEnableVertexAttribArray(1);

		GLintptr uvPtr = 6 * sizeof(float);
		glVertexAttribPointer(
			2,
			2,
			GL_FLOAT,
			GL_FALSE,
			14 * sizeof(GLfloat),
			(void*)uvPtr
		);

		glEnableVertexAttribArray(2);

		GLintptr tangnetPtr = 8 * sizeof(float);

		glVertexAttribPointer(
			3,
			3,
			GL_FLOAT,
			GL_FALSE,
			14 * sizeof(GLfloat),
			(void*)tangnetPtr
		);

		glEnableVertexAttribArray(3);

		GLintptr bitangnetPtr = 11 * sizeof(float);

		glVertexAttribPointer(
			4,
			3,
			GL_FLOAT,
			GL_FALSE,
			14 * sizeof(GLfloat),
			(void*)bitangnetPtr
		);

		glEnableVertexAttribArray(4);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
public:
	GLuint getNormTex() {
		return this->normTex;
	}

};