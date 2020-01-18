#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../include/shader.h"
#include "../include/camera.h"
#include "../include/skybox.h"
#include "../include/model.h"
#include "../include/circle.h"


#include <iostream>

using Skybox = Learus_Skybox::Skybox;
using Circle = Learus_Circle::Circle;

// Globals
bool animation = false;

// Timing
float lastFrame = 0.0f;
float frameToggled = 0.0f;
float timeSinceLastToggle = 1.0f;

// Some settings
const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 720;

float earthOrbitRadius = 100.0f;
float moonOrbitRadius = 20.0f;
glm::vec3 sunPos = glm::vec3(0.0f, -1.0f, 0.0f);
glm::vec3 earthPos = sunPos + glm::vec3(sin(frameToggled) * earthOrbitRadius, 0.0f, cos(frameToggled) * earthOrbitRadius);

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 30.0f));
float cameraOrbitRadius = 30.0f;
float rotateAngle = 1.0f;

// Mouse Input
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;



void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseInput(GLFWwindow * window, double xpos, double ypos);
void scrollInput(GLFWwindow * window, double xoffset, double yoffset);
void keyboardInput(GLFWwindow * window, float deltaTime);

int main()
{
    // Initialize and configure GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow * window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Solar System", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "ERROR: Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);


    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetScrollCallback(window, scrollInput);

    // Load glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader planetShader("./src/planet.vs", "./src/planet.fs");
    Shader sunShader("./src/sun.vs", "./src/sun.fs");

    // Load the models

    Model Sun("./models/Planet/planet.obj");
    Model Earth("./models/Earth/Globe.obj");
    Model Moon("./models/Rock/rock.obj");

    Circle EarthOrbitCircle(sunPos, earthOrbitRadius, glm::vec3(0.0f, 1.0f, 1.0f), 3000);
    Circle MoonOrbitCircle(earthPos, moonOrbitRadius, glm::vec3(1.0f, 1.0f, 0.0f), 3000);
    Skybox skyBox("./images/top.png", "./images/bottom.png", "./images/left.png", "./images/right.png", "./images/front.png", "./images/back.png");


    // Render Loop
    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        timeSinceLastToggle += deltaTime;
        if (animation)
            frameToggled += deltaTime;


        keyboardInput(window, deltaTime);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // view / projection
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        skyBox.setUniforms(projection, glm::mat4(glm::mat3(view)));
        skyBox.Draw();

        // Render the sun object
        sunShader.use();
        sunShader.setMat4("projection", projection);
        sunShader.setMat4("view", view);
        
        model = glm::translate(model, sunPos); // Center it (kinda)
        sunShader.setMat4("model", model);
        Sun.Draw(sunShader);


        planetShader.use();

        // Set the lighting
        planetShader.setVec3("viewPos", camera.Position);
        planetShader.setFloat("material.shininess", 32.0f);

        planetShader.setVec3("light.position", sunPos);
        planetShader.setVec3("light.ambient", 0.25f, 0.25f, 0.25f);
        planetShader.setVec3("light.diffuse", 1.8f, 1.8f, 1.8f);
        planetShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        planetShader.setFloat("pointLights[0].constant", 1.0f);
        planetShader.setFloat("pointLights[0].linear", 0.045);
        planetShader.setFloat("pointLights[0].quadratic", 0.0075);


        // Render the Earth object
        planetShader.setMat4("projection", projection);
        planetShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));

        
        // Orbit around the sun
        earthPos = sunPos + glm::vec3(sin(frameToggled) * earthOrbitRadius, 0.0f, cos(frameToggled) * earthOrbitRadius);
        model = glm::translate(model, earthPos);
        // Rotate around itself
        model = glm::rotate(model, frameToggled * 1.5f * glm::radians(-50.0f), glm::vec3(0.1f, 1.0f, 0.0f));

        planetShader.setMat4("model", model);
        Earth.Draw(planetShader);

        // Draw a circle showing the orbit around the sun
        EarthOrbitCircle.setUniforms(projection, view);
        EarthOrbitCircle.scale(glm::vec3(0.1f, 0.1f, 0.1f));
        EarthOrbitCircle.rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        EarthOrbitCircle.Draw();


        // Render the Moon object
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        // Orbit around the earth
        glm::vec3 moonPos = earthPos + glm::vec3(0.0f, sin(frameToggled) * moonOrbitRadius , cos(frameToggled) * moonOrbitRadius);
        model = glm::translate(model, moonPos);
        planetShader.use();
        planetShader.setMat4("model", model);
        Moon.Draw(planetShader);

        // Draw a circle showing the orbit around the earth
        MoonOrbitCircle.setUniforms(projection, view);
        MoonOrbitCircle.scale(glm::vec3(0.1f, 0.1f, 0.1f));
        MoonOrbitCircle.translate(earthPos);
        MoonOrbitCircle.rotate(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        MoonOrbitCircle.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// Handles user keyboard input. Supposed to be used every frame, so deltaTime can be calculated appropriately.
void keyboardInput(GLFWwindow * window, float deltaTime)
{

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Upwards Rotation
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.Orbit(UP, cameraOrbitRadius, rotateAngle);

    // Downwards Rotation
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.Orbit(DOWN, cameraOrbitRadius, rotateAngle);

    // Rightwards Rotation
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.Orbit(RIGHT, cameraOrbitRadius, rotateAngle);

    // Leftwards Rotation
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.Orbit(LEFT, cameraOrbitRadius, rotateAngle);


    // Pause / Start
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
    {
        if (timeSinceLastToggle > 0.2)
        {
            animation = !animation;
            timeSinceLastToggle = 0.0f;
        }
    }
        
}

// Handles mouse scroll wheel. Supposed to be used as the glfw scroll callback.
void scrollInput(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.Zoom(yoffset);
}