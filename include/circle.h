#include "../lib/glad/glad.h"
#include "../lib/glm/glm.hpp"
#include "../lib/glm/gtc/matrix_transform.hpp"
#include <math.h>
#include <vector>
#include <string>

#include "shader.h"

#ifndef M_PI
    #define M_PI 3.14159
#endif


namespace Learus_Circle
{
    const char * vertex_shader =    "#version 330 core\n"
                                    "layout (location = 0) in vec3 aPos;\n"
                                    "layout (location = 1) in vec3 aColor;\n"
                                    "out vec3 Color;\n"
                                    "uniform mat4 model;\n"
                                    "uniform mat4 view;\n"
                                    "uniform mat4 projection;\n"
                                    "void main()\n"
                                    "{\n"
                                    "    Color = aColor;\n"
                                    "    gl_Position = projection * view * model * vec4(aPos, 1.0)\n;"
                                    "}\0";

    const char * fragment_shader =  "#version 330 core\n"
                                    "out vec4 FragColor;\n"
                                    "in vec3 Color;\n"
                                    "void main()\n"
                                    "{"
                                    "    FragColor = vec4(Color, 1.0);\n"
                                    "}\0";

    class Circle
    {
        public:

            struct Vertex
            {
                glm::vec3 Position;
                glm::vec3 Color;
            };

            std::vector<Circle::Vertex> vertices;

            glm::vec3 Center;
            float Radius;

            glm::vec3 Color;

            Shader shader;
            unsigned int VAO;
            

            Circle(glm::vec3 _center, float _radius, glm::vec3 _color, unsigned int _num_vertices)
            : Center(_center), Radius(_radius), Color(_color), shader(vertex_shader, fragment_shader, true)
            {
                glGenVertexArrays(1, &VAO);
                glGenBuffers(1, &VBO);

                glBindVertexArray(VAO);
                glBindBuffer(GL_ARRAY_BUFFER, VBO);

                for (float angle = 0.0f; angle <= 2.0f * M_PI; angle += 2.0f * M_PI / _num_vertices)
                {
                    Vertex v;
                    v.Position = glm::vec3(Radius * cos(angle) + Center.x, Radius * sin(angle) + Center.y, 0);
                    v.Color = Color;
                    vertices.push_back(v);
                }

                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

                // Position
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
                
                // Color
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, Color)));
                glEnableVertexAttribArray(1);

                glBindVertexArray(0);
            }

            void Draw()
            {
                shader.use();
                shader.setMat4("projection", projection);
                shader.setMat4("view", view);
                shader.setMat4("model", model);

                glBindVertexArray(VAO);
                glDrawArrays(GL_LINE_LOOP, 0, vertices.size());
                glBindVertexArray(0);
            }

            void translate(glm::vec3 newPos)
            {
                model = glm::translate(model, newPos);
            }

            void rotate(glm::f32 angle, const glm::highp_vec3 &axis)
            {
                model = glm::rotate(model, angle, axis);
            }

            void scale(glm::vec3 newScale)
            {
                model = glm::scale(model, newScale);
            }

            void setUniforms(glm::mat4 _projection, glm::mat4 _view, glm::mat4 _model = glm::mat4(1.0f))
            {
                projection = _projection;
                view = _view;
                model = _model;
            }

        private:
            unsigned int VBO;

            glm::mat4 projection;
            glm::mat4 view;
            glm::mat4 model;
    };
}
