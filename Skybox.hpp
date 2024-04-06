#pragma once

class Skybox {
private:
	unsigned int skyboxTex;
	unsigned int skyboxVAO;
	unsigned int skyboxVBO;
	unsigned int skyboxEBO;

    GLuint fragShaderSkyBox;
    GLuint shaderProgSkyBox;

public:
	
	void loadSkyBox() {
        std::string facesSkybox[]{
            //"Skybox/right.png",
            //"Skybox/left.png",
            //"Skybox/up.png",
            //"Skybox/down.png",
            //"Skybox/front.png",
            //"Skybox/back.png"
            "Skybox/image3x2.png",
            "Skybox/image1x2.png",
            "Skybox/image2x1.png",
            "Skybox/image2x3.png",
            "Skybox/image2x2.png",
            "Skybox/image4x2.png"
        };

        glGenTextures(1, &this->skyboxTex);
        glBindTexture(GL_TEXTURE_CUBE_MAP, this->skyboxTex);

        //to avoid the skybox from pixelating when its too beig/small
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        for (unsigned int i = 0; i < 6; i++) {
            int W, H, skyCChannel;

            stbi_set_flip_vertically_on_load(false);

            unsigned char* data = stbi_load(facesSkybox[i].c_str(), &W, &H, &skyCChannel, 0);

            if (data) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0,
                    GL_RGBA,
                    W,
                    H,
                    0,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    data
                );
                stbi_image_free(data);
            }
        }
        stbi_set_flip_vertically_on_load(true);

        std::fstream skyboxVertSrc("Shaders/skybox.vert");
        std::stringstream skyboxVertBuff;
        skyboxVertBuff << skyboxVertSrc.rdbuf();

        std::string skyboxVerts = skyboxVertBuff.str();
        const char* sky_v = skyboxVerts.c_str();

        std::fstream skyboxFragSrc("Shaders/skybox.frag");
        std::stringstream skyboxFragBuff;
        skyboxFragBuff << skyboxFragSrc.rdbuf();

        std::string skyboxFragS = skyboxFragBuff.str();
        const char* skybox_f = skyboxFragS.c_str();

        //creating vertex shader
        GLuint vertexShaderSkyBox = glCreateShader(GL_VERTEX_SHADER);
        //assign the source to the vertex shader
        glShaderSource(vertexShaderSkyBox, 1, &sky_v, NULL);
        //Compile the Vertex Shader
        glCompileShader(vertexShaderSkyBox);

        this->fragShaderSkyBox = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragShaderSkyBox, 1, &skybox_f, NULL);
        glCompileShader(fragShaderSkyBox);

        //Creating shader program
        this->shaderProgSkyBox = glCreateProgram();
        glAttachShader(shaderProgSkyBox, vertexShaderSkyBox);
        glAttachShader(shaderProgSkyBox, fragShaderSkyBox);

        glLinkProgram(shaderProgSkyBox);

        glDeleteShader(vertexShaderSkyBox);
        glDeleteShader(fragShaderSkyBox);

        float skyboxVertices[]{
            -1.f, -1.f, 1.f, //0
            1.f, -1.f, 1.f,  //1
            1.f, -1.f, -1.f, //2
            -1.f, -1.f, -1.f,//3
            -1.f, 1.f, 1.f,  //4
            1.f, 1.f, 1.f,   //5
            1.f, 1.f, -1.f,  //6
            -1.f, 1.f, -1.f  //7
        };

        unsigned int skyboxIndices[]{
            1,2,6,
            6,5,1,

            0,4,7,
            7,3,0,

            4,5,6,
            6,7,4,

            0,3,2,
            2,1,0,

            0,1,5,
            5,4,0,

            3,7,6,
            6,2,3
        };

        glGenVertexArrays(1, &this->skyboxVAO);
        glGenBuffers(1, &this->skyboxVBO);
        glGenBuffers(1, &this->skyboxEBO);

        glBindVertexArray(this->skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, this->skyboxVBO);

        glBindVertexArray(this->skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, this->skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->skyboxEBO);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_INT) * 36, &skyboxIndices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
	}

    void update(glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {
        glm::mat4 sky_view = glm::mat4(1.f);
        sky_view = glm::mat4(glm::mat3(viewMatrix));

        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        glUseProgram(this->shaderProgSkyBox);

        unsigned int skyboxViewLoc = glGetUniformLocation(this->shaderProgSkyBox, "view");
        glUniformMatrix4fv(skyboxViewLoc, 1, GL_FALSE, glm::value_ptr(sky_view));

        unsigned int skyboxProjLoc = glGetUniformLocation(this->shaderProgSkyBox, "projection");
        glUniformMatrix4fv(skyboxProjLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        glBindVertexArray(this->skyboxVAO);

        glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_CUBE_MAP, this->skyboxTex);

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glDepthMask(GL_TRUE);

        glDepthFunc(GL_LESS);
    }

};