#pragma once
class light {
public:
    glm::vec3 lightPos;
    glm::vec3 lightColor;

    float ambientStr;
    glm::vec3 ambientColor;

    float specStr;
    float specPhong;

public:

    light() {
        this->lightPos = glm::vec3(5, 0, 0);
        this->lightColor = glm::vec3(1, 1, 1);
        this->ambientStr = 0.1f;
        this->ambientColor = lightColor;
        this->specStr = 0.5f;
        this->specPhong = 16.f;
    }

    void setLightPosition(glm::vec3 position) {
        this->lightPos = position;
    }

    void setLightColor(glm::vec3 color) {
        this->lightColor = color;
        this->ambientColor = color;
    }

    void setAmbientStrength(float strength) {
        this->ambientStr = strength;
    }

    void setSpecularStrength(float strength) {
        this->specStr = strength;
    }

    void setSpecularPhong(float phong) {
        this->specPhong = phong;
    }

    void setLight(GLuint shaderProg, glm::vec3 cameraPos) {

        GLuint lightAddress = glGetUniformLocation(shaderProg, "lightPos");
        glUniform3fv(lightAddress, 1, glm::value_ptr(this->lightPos));

        GLuint lightColorAddress = glGetUniformLocation(shaderProg, "lightColor");
        glUniform3fv(lightColorAddress, 1, glm::value_ptr(this->lightColor));

        GLuint ambientStrAddress = glGetUniformLocation(shaderProg, "ambientStr");
        glUniform1f(ambientStrAddress, this->ambientStr);

        GLuint ambientColorAddress = glGetUniformLocation(shaderProg, "ambientColor");
        glUniform3fv(ambientColorAddress, 1, glm::value_ptr(this->ambientColor));

        GLuint cameraPosAddress = glGetUniformLocation(shaderProg, "cameraPos");
        glUniform3fv(cameraPosAddress, 1, glm::value_ptr(cameraPos));

        GLuint specStrAddress = glGetUniformLocation(shaderProg, "specStr");
        glUniform1f(specStrAddress, this->specStr);

        GLuint specPhongAddress = glGetUniformLocation(shaderProg, "specPhong");
        glUniform1f(specPhongAddress, this->specPhong);
    }
};