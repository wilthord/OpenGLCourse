#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include "Camera.h"
#include "Texture.h"
#include "Light.h"

//Texture objects
Texture brickTexture;
Texture dirtTexture;

//Window Dimensions
const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = M_PI / 180.0f;

//Delta time values
GLfloat deltaTime, lastTime = 0.0f;

std::vector<Mesh*> meshList;
std::vector<Shader*> shaderList;

Light mainLight;

Window mainWindow;

// Vertex shader
static const char* vShader = "shaders/shader.vert";

// Fragment shader
static const char* fShader = "shaders/shader.frag";

void createTriangle() {
    unsigned int indices[] = {
        0, 3, 1,
        1, 3, 2,
        2, 3, 0,
        0, 1, 2
    };

    GLfloat vertices[] = {
    //    x      y     z          u     v
        -1.0f, -1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, -1.0f, 1.0f,      0.5f, 0.0f,
        1.0f, -1.0f, 0.0f,      1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,       0.5f, 1.0f
    };
    
    Mesh* obj1 = new Mesh();
    obj1->createMesh(vertices, indices, 20, 12);
    meshList.push_back(obj1);

    Mesh* obj2 = new Mesh();
    obj2->createMesh(vertices, indices, 20, 12);
    meshList.push_back(obj2);

}

void createShader() {
    Shader *shader1 = new Shader();
    shader1->createFromFile(vShader, fShader);
    shaderList.push_back(shader1);
}

int main()
{
    mainWindow = Window(WIDTH, HEIGHT);
    mainWindow.initialize();

    createTriangle();
    createShader();

    Camera camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.3f);
    brickTexture = Texture((char*)"Textures/Tile.png");
    brickTexture.loadTexture();
    dirtTexture = Texture((char*)"Textures/Dirt.png");
    dirtTexture.loadTexture();

    mainLight = Light(1.0f, 1.0f, 1.0f, 0.7f);

    glm::mat4 projection = glm::perspective(45.0f, mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

    GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;
    GLuint uniformAmbientColour = 0, uniformAmbientIntensity = 0;
    // Loop until windows closed
    while (!mainWindow.getShouldClose()) {
        //Updating delta time
        GLfloat now = (GLfloat)glfwGetTime();
        deltaTime = now - lastTime;
        lastTime = now;
        
        // Get  + Handle user input events
        glfwPollEvents();

        camera.keyControl(mainWindow.getKeys(), deltaTime);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

        // Clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderList[0]->useShader();
        uniformModel = shaderList[0]->getModelLocation();
        uniformProjection = shaderList[0]->getProjectionLocation();
        uniformView = shaderList[0]->getViewLocation();

        //Getting Light uniforms
        uniformAmbientColour = shaderList[0]->getAmbientColourLocation();
        uniformAmbientIntensity = shaderList[0]->getAmbientIntensityLocation();
        mainLight.useLight(uniformAmbientIntensity, uniformAmbientColour);

        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
        brickTexture.useTexture();
        meshList[0]->renderMesh();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 1.0f, -2.5f));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        dirtTexture.useTexture();
        meshList[1]->renderMesh();

        glUseProgram(0);

        mainWindow.swapBuffers();
    }

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu