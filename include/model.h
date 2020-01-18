#ifndef MODEL_H
#define MODEL_H

#include "../lib/glad/glad.h"
#include <GLFW/glfw3.h>

#include "../lib/glm/glm.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#ifndef STB_IMAGE_IMPLEMENTATION
    #define STB_IMAGE_IMPLEMENTATION
    #include "../lib/stbi/stb_image.h"
#endif



#include "mesh.h"

#include <iostream>
#include <vector>
#include <string>

unsigned int TextureFromFile(const char *path, const std::string &directory);

class Model
{
    public:
        // Methods

        Model(const char * path)
        {
            loadModel(path);
        }

        void Draw(Shader shader)
        {
            for (unsigned int i = 0; i < meshes.size(); i++)
            {
                meshes[i].Draw(shader);
            }
        }

    private:
        // Model Data
        std::vector<Mesh> meshes;
        std::vector<Texture> textures_loaded;
        std::string directory;
        

        // Methods
        void loadModel(std::string path)
        {
            Assimp::Importer importer;
            const aiScene * scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            {
                std::cout << "Error while importing model: " << importer.GetErrorString() << std::endl;
                return;
            }

            directory = path.substr(0, path.find_last_of('/'));

            processNode(scene->mRootNode, scene);
        }

        void processNode(aiNode * node, const aiScene * scene)
        {
            // Process all meshes in current node
            for (unsigned int i = 0; i < node->mNumMeshes; i++)
            {
                aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
                meshes.push_back(processMesh(mesh, scene));
            }

            // Process children
            for (unsigned int i = 0; i < node->mNumChildren; i++)
            {
                processNode(node->mChildren[i], scene);
            }
        }

        Mesh processMesh(aiMesh * mesh, const aiScene * scene)
        {
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            std::vector<Texture> textures;

            // Process vertices
            for (unsigned int i = 0; i < mesh->mNumVertices; i++)
            {
                Vertex vertex;
                glm::vec3 v;

                // Position attribute
                v.x = mesh->mVertices[i].x;
                v.y = mesh->mVertices[i].y;
                v.z = mesh->mVertices[i].z;
                vertex.Position = v;

                v.x = mesh->mNormals[i].x;
                v.y = mesh->mNormals[i].y;
                v.z = mesh->mNormals[i].z;
                vertex.Normal = v;

                if (mesh->mTextureCoords[0])
                {
                    // We are saving only the first set of textures atm
                    glm::vec2 v;
                    v.x = mesh->mTextureCoords[0][i].x;
                    v.y = mesh->mTextureCoords[0][i].y;
                    vertex.TexCoords = v;
                }
                else
                {
                    vertex.TexCoords = glm::vec2(0.0f, 0.0f);
                }
                
                vertices.push_back(vertex);
            }

            // Process indices
            for (unsigned int i = 0; i < mesh->mNumFaces; i++)
            {
                aiFace face = mesh->mFaces[i];
                for (unsigned int j = 0; j < face.mNumIndices; j++)
                {
                    indices.push_back(face.mIndices[j]);
                }
            }

            // Process material
            if (mesh->mMaterialIndex >= 0)
            {
                aiMaterial * material = scene->mMaterials[mesh->mMaterialIndex];

                std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
                textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

                std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
                textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            }

            return Mesh(vertices, indices, textures);
        }

        std::vector<Texture> loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string typeName)
        {
            std::vector<Texture> textures;

            for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
            {
                aiString str;
                mat->GetTexture(type, i, &str);

                bool skip = false;

                // If we have loaded this texture before do not load it again
                for (unsigned int j = 0; j < textures_loaded.size(); j++)
                {
                    if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                    {
                        textures.push_back(textures_loaded[j]);
                        skip = true;
                        break;
                    }
                }

                if (skip) continue;

                Texture texture;

                texture.id = TextureFromFile(str.C_Str(), directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);

                textures_loaded.push_back(texture);
            }

            return textures;
        }

        unsigned int TextureFromFile(const char *path, const std::string &directory)
        {
            std::string filename = std::string(path);
            filename = directory + '/' + filename;

            unsigned int textureID;
            glGenTextures(1, &textureID);

            int width, height, nrComponents;
            unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
            if (data)
            {
                GLenum format;
                if (nrComponents == 1)
                    format = GL_RED;
                else if (nrComponents == 3)
                    format = GL_RGB;
                else if (nrComponents == 4)
                    format = GL_RGBA;

                glBindTexture(GL_TEXTURE_2D, textureID);
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                stbi_image_free(data);
            }
            else
            {
                std::cout << "Texture failed to load at path: " << path << std::endl;
                stbi_image_free(data);
            }

            return textureID;
        }
};



#endif