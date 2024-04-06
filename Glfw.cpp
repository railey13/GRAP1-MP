#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Model3D.hpp"
#include "MyCamera.hpp"
#include "light.hpp"
#include "direction.hpp"
#include "pointlight.hpp"
#include "Skybox.hpp"

#define CAMERA1_POS glm::vec3(-camX, camY, camZ)
#define CAMERA2_POS orthoPos

#define WINDOW_HEIGHT 1000.f
#define WINDOW_WIDTH 1000.f

#define RADIUS 15.f  // Adjust as needed

/*

NOTE: Cannot draw objects with different shaderprogs at the same time, will only draw which is drawn last

FINAL NOTE: Cannot make normal mapping work. with different shaders. a model that is being drawn first with 
            a different shader program is being overwritten or not being drawn
            Player class is supposed to handle the creation of player with normal mapping
            Deleted playerSample.vert/.frag and playerShaderProg
            Just did normal lighting 

            Point light position is not being changed in the scene even though in the code it is
*/

/////////////////////////////////////////////////////////////////////////////////////////////

float pitch = 0.f; //rotation on the horizontal axis (camera)
float yaw = -180.f; //rotation on the vertical axis (camera)
//we just set it to -180.f so that the first object spawn will look 
//directly at the camera

glm::vec3 playerPosition = glm::vec3(0.f, -20.f, 0.f); //for position of our object/s
glm::vec3 lighPosition1 = glm::vec3(5.f, -5.f, 5.f); //for light1

glm::vec3 forwardVector;

float lastX = 500.f, lastY = 500.f; //will set the mouse curser to center of screen

float camX = 0.f;
float camY = 0.f;
float camZ = 0.f;

float theta_y_mod = 0.f;

float orbitRadius = 5.0f; // Adjust the radius as needed
float Speed = 5.f; // Adjust the speed of rotation as needed
float rotationAngleX = 0.0f; // Initial angle for rotation around x-axis
float rotationAngleY = 0.0f; // Initial angle for rotation around y-axis
float rotationAngleZ = 0.0f; // Initial angle for rotation around z-axis

float deltaTime = 0.f;
float lastFrame = 0.f;

float currentFrame;
float deltaSpawnTime = 0.f;
float deltaLastFrame = 0.f;
float currentFrameTime = 0.f;
float fSpawnCD = 0.f;

float firstSet = true;

float click = false;

PerspectiveCamera firstPerson;
PerspectiveCamera thirdPerson;
OrthographicCamera ortho;

std::vector<Model3D*> models;

glm::vec3 orthoPos = ortho.getCameraPos();

/////////////////////////////////////////////////////////////////////////////////////////////

void Key_CallBack(GLFWwindow* window, //pointer to the window
    int key, //keycode of the press
    int scancode, //physical position of the press
    int action, //either press / release
    int mods) {// which modifier keys is held down

    float movementSpeed = 60.f * deltaTime;
    float objectTurnSpeed = 3.f;

    float directionX = cos(glm::radians(theta_y_mod));
    float directionZ = sin(glm::radians(theta_y_mod));

    if (key == GLFW_KEY_1 && !firstPerson.getCamera() && fSpawnCD <= 0) { //move left
        firstPerson.setCamera(true);
        thirdPerson.setCamera(false);
        ortho.setCamera(false);
        fSpawnCD = 0.2f;
    }
    if (key == GLFW_KEY_1 && !thirdPerson.getCamera() && fSpawnCD <= 0) {
        thirdPerson.setCamera(true);
        firstPerson.setCamera(false);
        ortho.setCamera(false);
        fSpawnCD = 0.2f;
    }
    if (key == GLFW_KEY_2 && !ortho.getCamera()) { //move right
        ortho.setCamera(true);
        firstPerson.setCamera(false);
        thirdPerson.setCamera(false);
        firstSet = true;
    }

    if (!ortho.getCamera()) {
        if (key == GLFW_KEY_A) {
            theta_y_mod += objectTurnSpeed;
        }
        if (key == GLFW_KEY_D) {
            theta_y_mod -= objectTurnSpeed;
        }
        if (key == GLFW_KEY_W) {
            playerPosition.x += movementSpeed * directionX;
            playerPosition.z -= movementSpeed * directionZ;
        }
        if (key == GLFW_KEY_S) {
            playerPosition.x -= movementSpeed * directionX;
            playerPosition.z += movementSpeed * directionZ;
        }
        if (key == GLFW_KEY_Q) {
            playerPosition.y += movementSpeed;
        }
        if (key == GLFW_KEY_E) {
            playerPosition.y -= movementSpeed;
        }
    }
    else {
        if (key == GLFW_KEY_UP) {
            orthoPos.x += movementSpeed;
        }
        if (key == GLFW_KEY_DOWN) {
            orthoPos.x -= movementSpeed;
        }
        if (key == GLFW_KEY_LEFT) {
            orthoPos.z -= movementSpeed;
        }
        if (key == GLFW_KEY_RIGHT) {
            orthoPos.z += movementSpeed;
        }

    }

    if (playerPosition.y >= -1.f) {
        playerPosition.y = -1.f;
    }

    if (key == GLFW_KEY_ESCAPE) { //close window
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void Mouse_CallBack(GLFWwindow* window,
    double xPos,
    double yPos) {

    float xoffSet = xPos - lastX;
    float yoffSet = lastY - yPos;    //gets the direction where the camera faced
    lastX = xPos;
    lastY = yPos;   //sets the last known position to the current position

    const float sensitivity = 0.4f;

    xoffSet *= sensitivity;
    yoffSet *= sensitivity; //multiply the movement with sensitivity

    if (click) {
        yaw += xoffSet;
        pitch += yoffSet;   //adding the movements to the yaw and pitch;
    }

    if (pitch > 89.f) {
        pitch = 89.f;
    }

    if (pitch < -89.f) { //maintain at 90 and -90 for rotation in horizontal axis (up and down)
        pitch = -89.f;
    }
}

void onMouseClick(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        click = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        click = false;
    }
}

int main(void)

{
    std::cout << " Hello World";

    GLFWwindow* window;
    int colorIndex = 0;
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Graphfinalprojecct", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    firstPerson.setCamera(true);
    thirdPerson.setCamera(false);
    ortho.setCamera(false);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL();

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*****************************************
     *                                       *
     *          Making the Sky Box           *
     *                                       *
     *****************************************/

    Skybox skybox;

    skybox.loadSkyBox();

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::fstream vertSrc("Shaders/sample.vert");
    std::stringstream vertBuff;
    vertBuff << vertSrc.rdbuf();

    std::string vertS = vertBuff.str();
    const char* v = vertS.c_str();

    std::fstream fragSrc("Shaders/sample.frag");
    std::stringstream fragBuff;
    fragBuff << fragSrc.rdbuf();

    std::string fragS = fragBuff.str();
    const char* f = fragS.c_str();

    //creating vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //assign the source to the vertex shader
    glShaderSource(vertexShader, 1, &v, NULL);
    //Compile the Vertex Shader
    glCompileShader(vertexShader);

    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &f, NULL);
    glCompileShader(fragShader);

    ////Creating shader program
    GLuint shaderProg = glCreateProgram();
    glAttachShader(shaderProg, vertexShader);
    glAttachShader(shaderProg, fragShader);

    glLinkProgram(shaderProg);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*****************************************
    *                                       *
    *          Listening for Mouse          *
    *                 and                   *
    *            Keyboard Input             *
    *                                       *
    *****************************************/

    glfwSetCursorPosCallback(window, Mouse_CallBack);   //calls function for updating the view of the camera
    glfwSetMouseButtonCallback(window, onMouseClick);
    glfwSetKeyCallback(window, Key_CallBack); //calls function for updating x,y,z of the camera

    directionlight directionlight(glm::vec3(4.0f, -5.f, 0.f));
    pointlight pointlight(5.0f);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    auto firstModel = new Model3D(playerPosition, "3D/fighter_text1.png", "3D/Plundernauts_fighter.obj", glm::vec3(2.f, 2.f, 2.f), 1);
    firstModel->spawn();
    auto secondModel = new Model3D(glm::vec3(25.f, -20.f, 10.f), "3D/Cruiser2.png", "3D/Cruiser2.obj", glm::vec3(1.0f, 1.0f, 1.0f), 1);
    secondModel->spawn();
    auto thirdtModel = new Model3D(glm::vec3(25.f, -15.f, 0.f), "3D/cruiser1.png", "3D/Cruiser1.obj", glm::vec3(0.5f, 0.5f, 0.5f), 1);
    thirdtModel->spawn();
    auto fourthModel = new Model3D(glm::vec3(35.f, -10.f,-19.f), "3D/Transport_Text.png", "3D/Transport.obj", glm::vec3(0.5f, 0.5f, 0.5f), 1);
    fourthModel->spawn();
    auto fifthModel = new Model3D(glm::vec3(40.f, -20.f, 20.f), "3D/Corv_Missileboat_Text.png", "3D/Corvette_MissileBoat.obj", glm::vec3(0.5f, 0.5f, 0.5f), 1);
    fifthModel->spawn();
    auto sixthModel = new Model3D(glm::vec3(50.f, -30.f, 12.f), "3D/Corv1Text.png", "3D/Corvette_Interceptor_1.obj", glm::vec3(0.5f, 0.5f, 0.5f), 1);
    sixthModel->spawn();
    auto seventhModel = new Model3D(glm::vec3(55.f, -22.f, -10.f), "3D/science_text.png", "3D/Science_Ship.obj", glm::vec3(0.5f, 0.5f, 0.5f), 1);
    seventhModel->spawn();

    models.push_back(firstModel);
    models.push_back(secondModel);
    models.push_back(thirdtModel);
    models.push_back(fourthModel);
    models.push_back(fifthModel);
    models.push_back(sixthModel);
    models.push_back(seventhModel);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*****************************************
     *                                       *
     * Loop until the user closes the window *
     *                                       *
     *****************************************/

    while (!glfwWindowShouldClose(window))
    {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        fSpawnCD -= deltaTime;

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        models[0]->setYTheta(theta_y_mod);
        models[0]->setPosition(playerPosition);

        std::cout << "Object on Y: " << models[0]->getPosition().y << std::endl;

        forwardVector = glm::vec3(
            cos(glm::radians(models[0]->getThetas().y)) * cos(glm::radians(models[0]->getThetas().x)),
            sin(glm::radians(models[0]->getThetas().x)),
            -sin(glm::radians(models[0]->getThetas().y)) * cos(glm::radians(models[0]->getThetas().x))
        );

        forwardVector = normalize(forwardVector);
        if (firstPerson.getCamera()) {
            firstPerson.setCameraPos(models[0]->getPosition() + forwardVector);
            firstPerson.setProjectionMatrix(WINDOW_WIDTH, WINDOW_HEIGHT, 0);
            firstPerson.setViewMatrix(firstPerson.getCameraPos() + forwardVector);
            firstPerson.projection_viewMatrix(shaderProg);
            skybox.update(firstPerson.getViewMatrix(), firstPerson.getProjectionMatrix());
            directionlight.ambientColor = glm::vec3(0, 3, 0);
            directionlight.lightColor = glm::vec3(0, 3, 0);

            pointlight.ambientColor = glm::vec3(0, 3, 0);
            pointlight.lightColor = glm::vec3(0, 3, 0);

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        }
        if (thirdPerson.getCamera()) {
            camX = cos(glm::radians(yaw + theta_y_mod)) * cos(glm::radians(pitch)) * -RADIUS;
            camY = sin(glm::radians(pitch)) * -RADIUS;
            camZ = sin(glm::radians(yaw + theta_y_mod)) * cos(glm::radians(pitch)) * -RADIUS;

            thirdPerson.setCameraPos(CAMERA1_POS + models[0]->getPosition());
            thirdPerson.setProjectionMatrix(WINDOW_WIDTH, WINDOW_HEIGHT, 1);
            thirdPerson.setViewMatrix(models[0]->getPosition());
            thirdPerson.projection_viewMatrix(shaderProg);
            skybox.update(thirdPerson.getViewMatrix(), thirdPerson.getProjectionMatrix());

            directionlight.ambientColor = glm::vec3(1,1,1);
            directionlight.lightColor = glm::vec3(1, 1, 1);

            pointlight.ambientColor = glm::vec3(1, 1, 1);
            pointlight.lightColor = glm::vec3(1, 1, 1);

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        if (ortho.getCamera()) {
            if (firstSet) {
                orthoPos = models[0]->getPosition();
                ortho.setCameraPos(orthoPos);

                firstSet = false;
            }
            ortho.setCameraPos(orthoPos);
            ortho.setProjectionMatrix();
            ortho.setViewMatrix(ortho.getCameraPos() + glm::vec3(0.1f, -1.f, 0.0f));
            ortho.projection_viewMatrix(shaderProg);
            skybox.update(ortho.getViewMatrix(), firstPerson.getProjectionMatrix());

            directionlight.ambientColor = glm::vec3(1, 1, 1);
            directionlight.lightColor = glm::vec3(1, 1, 1);

            pointlight.ambientColor = glm::vec3(1, 1, 1);
            pointlight.lightColor = glm::vec3(1, 1, 1);

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        for (auto model : models) { //will update every objects inside the models vector
            glUseProgram(shaderProg);
            model->update(shaderProg);

            glBindVertexArray(model->getVAO());

            glActiveTexture(GL_TEXTURE0);
            GLuint tex0Address = glGetUniformLocation(shaderProg, "tex0");
            glBindTexture(GL_TEXTURE_2D, model->getTexture());
            glUniform1i(tex0Address, 0);


            model->draw();
            
        }
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        {
            colorIndex++; // Increment color index when F key is pressed

            // Cycle between 1, 4, 7 for directionlight.lightColor
            switch (colorIndex % 3)
            {
            case 0:
                pointlight.brightness = 1; // brightness 1
                break;
            case 1:
                pointlight.brightness = 4; // brightness 4
                break;
            case 2:
                pointlight.brightness = 7; // brightness 7
                break;
            }
        }
        pointlight.setLightPosition(models[0]->getPosition() + forwardVector);
        pointlight.setbrightness(shaderProg);
        pointlight.setLight(shaderProg, firstPerson.getCameraPos());

        directionlight.direction = glm::vec3(0.f, -1.f, 0.f);
        directionlight.lightPos = glm::vec3(2.f, 100.f, 1.f);
        directionlight.setLight(shaderProg, CAMERA1_POS);
        directionlight.setdirection(shaderProg);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}