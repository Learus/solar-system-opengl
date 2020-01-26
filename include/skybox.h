#include "../lib/glad/glad.h"
#include <string>
#include "shader.h"

#ifndef STB_IMAGE_IMPLEMENTATION
    #define STB_IMAGE_IMPLEMENTATION
    #include "../lib/stbi/stb_image.h"
#endif


namespace Learus_Skybox
{
    const char * vertex_shader =    "#version 330 core\n"
                                    "layout (location = 0) in vec3 aPos;\n"
                                    "out vec3 TexCoords;\n"
                                    "uniform mat4 projection;\n"
                                    "uniform mat4 view;\n"
                                    "void main() {\n"
                                    "   TexCoords = aPos;\n"
                                    "   gl_Position = projection * view * vec4(aPos, 1.0);\n"
                                    "}\0";

    const char * fragment_shader =  "#version 330 core\n"
                                    "out vec4 FragColor;\n"
                                    "in vec3 TexCoords;\n"
                                    "uniform samplerCube skybox;\n"
                                    "void main() {\n"
                                    "   FragColor = texture(skybox, TexCoords);\n"
                                    "}\0";
                                    
    class Skybox
    {
        public:
            unsigned int textureID;
            unsigned int VAO;
            Shader shader;

            Skybox(std::string top, std::string bottom, std::string left, std::string right, std::string front, std::string back)
            : shader(Learus_Skybox::vertex_shader, Learus_Skybox::fragment_shader, true)
            {
                // Create Vertices of the cube, VBO, VAO
                float vertices[] = {
                    // positions          
                    -1.0f,  1.0f, -1.0f,
                    -1.0f, -1.0f, -1.0f,
                    1.0f, -1.0f, -1.0f,
                    1.0f, -1.0f, -1.0f,
                    1.0f,  1.0f, -1.0f,
                    -1.0f,  1.0f, -1.0f,

                    -1.0f, -1.0f,  1.0f,
                    -1.0f, -1.0f, -1.0f,
                    -1.0f,  1.0f, -1.0f,
                    -1.0f,  1.0f, -1.0f,
                    -1.0f,  1.0f,  1.0f,
                    -1.0f, -1.0f,  1.0f,

                    1.0f, -1.0f, -1.0f,
                    1.0f, -1.0f,  1.0f,
                    1.0f,  1.0f,  1.0f,
                    1.0f,  1.0f,  1.0f,
                    1.0f,  1.0f, -1.0f,
                    1.0f, -1.0f, -1.0f,

                    -1.0f, -1.0f,  1.0f,
                    -1.0f,  1.0f,  1.0f,
                    1.0f,  1.0f,  1.0f,
                    1.0f,  1.0f,  1.0f,
                    1.0f, -1.0f,  1.0f,
                    -1.0f, -1.0f,  1.0f,

                    -1.0f,  1.0f, -1.0f,
                    1.0f,  1.0f, -1.0f,
                    1.0f,  1.0f,  1.0f,
                    1.0f,  1.0f,  1.0f,
                    -1.0f,  1.0f,  1.0f,
                    -1.0f,  1.0f, -1.0f,

                    -1.0f, -1.0f, -1.0f,
                    -1.0f, -1.0f,  1.0f,
                    1.0f, -1.0f, -1.0f,
                    1.0f, -1.0f, -1.0f,
                    -1.0f, -1.0f,  1.0f,
                    1.0f, -1.0f,  1.0f
                };

                glGenVertexArrays(1, &VAO);
                glGenBuffers(1, &VBO);

                glBindVertexArray(VAO);

                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                // Position
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

                glBindVertexArray(0);


                // Bind Textures
                glGenTextures(1, &textureID);
                glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

                loadTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, front);
                loadTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, back);
                loadTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, top);
                loadTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, bottom);
                loadTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X, right);
                loadTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, left);
                
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            }

            void Draw()
            {
                glDepthMask(GL_FALSE);

                shader.use();
                shader.setMat4("projection", projection);
                shader.setMat4("view", view);
                
                glBindVertexArray(VAO);
                glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
                glDrawArrays(GL_TRIANGLES, 0, 36);

                glDepthMask(GL_TRUE);
            }

            void setUniforms(glm::mat4 _projection, glm::mat4 _view)
            {
                projection = _projection;
                view = _view;
            }

        private:

            unsigned int VBO;

            glm::mat4 projection;
            glm::mat4 view;

            void loadTexture(GLenum target, std::string path)
            {
                int width, height, nrChannels;
                unsigned char * data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

                if (data)
                {
                    glTexImage2D(target, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                }
                else
                {
                    std::cerr << "ERROR: Cubemap texture failed to load at path: " << path << std::endl;
                }

                stbi_image_free(data);
            }
    };
}
