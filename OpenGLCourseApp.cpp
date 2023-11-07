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
#include "Material.h"

//Texture objects
Texture brickTexture;
Texture dirtTexture;

//Window Dimensions
const GLint WIDTH = 1366, HEIGHT = 768;
const float toRadians = M_PI / 180.0f;

//Delta time values
GLfloat deltaTime, lastTime = 0.0f;

std::vector<Mesh*> meshList;
std::vector<Shader*> shaderList;

Light mainLight;

Material shinyMaterial;
Material dullMaterial;

Window mainWindow;

// Vertex shader
static const char* vShader = "shaders/shader.vert";

// Fragment shader
static const char* fShader = "shaders/shader.frag";


void calcAverageNormals(unsigned int * indices, unsigned int indiceCount, GLfloat * vertices, unsigned int verticeCount,
                        unsigned int vLength, unsigned int normalOffset) {

    for (size_t i = 0; i < indiceCount; i += 3) {
        unsigned int in0 = indices[i] * vLength;
        unsigned int in1 = indices[i + 1] * vLength;
        unsigned int in2 = indices[i + 2] * vLength;

        glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
        glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
        glm::vec3 normal = glm::cross(v1, v2);
        normal = glm::normalize(normal);

        in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
        vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
        vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
        vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
    }
    
    for (size_t i = 0; i < verticeCount / vLength; i++) {
        unsigned int nOffset = i * vLength + normalOffset;
        glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
        vec = glm::normalize(vec);
        vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
    }
    
}

void createTriangle() {
    unsigned int indices[] = {
        0, 3, 1,
        1, 3, 2,
        2, 3, 0,
        0, 1, 2
    };

    GLfloat vertices[] = {
    //    x      y     z          u     v        nx    ny    nz
        -1.0f, -1.0f, 0.0f,     0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 1.0f,      0.5f, 0.0f,     0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f,      1.0f, 0.0f,     0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,       0.5f, 1.0f,     0.0f, 0.0f, 0.0f
    };

    calcAverageNormals(indices, 12, vertices, 32, 8, 5);
    
    Mesh* obj1 = new Mesh();
    obj1->createMesh(vertices, indices, 32, 12);
    meshList.push_back(obj1);

    Mesh* obj2 = new Mesh();
    obj2->createMesh(vertices, indices, 32, 12);
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

    shinyMaterial = Material(1.0f, 32);
    dullMaterial = Material(0.3f, 4);

    mainLight = Light(1.0f, 1.0f, 1.0f, 0.7f, 
        2.0f, -1.0f, -2.0f, 1.0f);

    glm::mat4 projection = glm::perspective(45.0f, mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

    GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0;
    GLuint uniformAmbientColour = 0, uniformAmbientIntensity = 0;
    GLuint uniformDirection = 0, uniformDiffuseIntensity = 0;
    GLuint uniformSpecularIntensity = 0, uniformShine = 0;
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
        uniformDiffuseIntensity = shaderList[0]->getDiffuseIntensityLocation();
        uniformDirection = shaderList[0]->getDirectionLocation();

        //Specular light uniforms
        uniformEyePosition = shaderList[0]->getEyePositionLocation();
        uniformSpecularIntensity = shaderList[0]->getSpecularIntensityLocation();
        uniformShine = shaderList[0]->getShininessLocation();

        mainLight.useLight(uniformAmbientIntensity, uniformAmbientColour, 
            uniformDiffuseIntensity, uniformDirection);

        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
        glUniform3f(uniformEyePosition, camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);

        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        brickTexture.useTexture();
        shinyMaterial.useMaterial(uniformSpecularIntensity, uniformShine);
        meshList[0]->renderMesh();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 1.0f, -2.5f));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        dirtTexture.useTexture();
        dullMaterial.useMaterial(uniformSpecularIntensity, uniformShine);
        meshList[1]->renderMesh();

        glUseProgram(0);

        mainWindow.swapBuffers();
    }

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu