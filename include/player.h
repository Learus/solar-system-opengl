#ifndef PLAYER_H
#define PLAYER_H

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "camera.h"




class Player
{
    public:

        enum CameraStyle
        {
            FPS,
            FLY,
            LOOK
        };

        Camera camera;

        // Methods
        Player(unsigned int _screenWidth, unsigned int _screenHeight, CameraStyle style)
        : screenWidth(_screenWidth), screenHeight(_screenHeight)
        {
            
        }

        glm::vec3 Position()
        {
            return camera.Position;
        }

        // Handles user keyboard input. Supposed to be used every frame, so deltaTime can be calculated appropriately. CHANGE THIS TO YOUR LIKING.
        void keyboard_input(GLFWwindow * window, float deltaTime)
        {

            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                camera.Move(FORWARD, deltaTime);

            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                camera.Move(BACKWARD, deltaTime);

            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                camera.Move(LEFT, deltaTime);

            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                camera.Move(RIGHT, deltaTime);
            
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
                camera.Move(UP, deltaTime);

            if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
                camera.Move(DOWN, deltaTime);
        }

        // Handles mouse buttons. Supposed to be used as the glfw mouse callback. CHANGE THIS TO YOUR LIKING.
        void mouse_input(GLFWwindow * window, double xpos, double ypos)
        {
            if (firstMouse)
            {
                lastX = xpos;
                lastY = ypos;
                firstMouse = false;
            }

            float xoffset = xpos - lastX;
            float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

            lastX = xpos;
            lastY = ypos;

            camera.Rotate(xoffset, yoffset);
        }

        // Handles mouse scroll wheel. Supposed to be used as the glfw scroll callback. CHANGE THIS TO YOUR LIKING.
        void scroll_input(GLFWwindow* window, double xoffset, double yoffset)
        {
            camera.Zoom(yoffset);
        }

    private:
        const unsigned int screenWidth = 800;
        const unsigned int screenHeight = 600;

        float lastX = screenWidth / 2.0f;
        float lastY = screenHeight / 2.0f;

        bool firstMouse = true;
};

#endif