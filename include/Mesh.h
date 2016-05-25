#pragma once
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;
// GL Includes
#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Vertex{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 textureCoor;
};

struct Texture{
    GLuint id;
    string type;
    aiString path;
};

class Mesh{
    public:
        vector<Vertex> vertices;
        vector<GLuint> indices;
        vector<Texture> textures;
        //Constructor
        Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures){
            this->vertices = vertices;
            this->indices = indices;
            this->textures = textures;

            this->setupMesh();
        }
        void Draw(Shader shader){
            GLuint diffuseNr = 1;
            GLuint specularNr = 1;
            for(GLuint i = 0; i < this->textures.size(); i++)
            {
                glActiveTexture(GL_TEXTURE0 + i); // Activate proper texture unit before binding
                // Retrieve texture number (the N in diffuse_textureN)
                stringstream ss;
                string number;
                string name = this->textures[i].type;
                if(name == "texture_diffuse")
                    ss << diffuseNr++; // Transfer GLuint to stream
                else if(name == "texture_specular")
                    ss << specularNr++; // Transfer GLuint to stream
                number = ss.str();
                glUniform1f(glGetUniformLocation(shader.Program,
                        (name + number).c_str()), i);
                glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
            }
            //glUniform1i(glGetUniformLocation(shader.Program,
            //        "number"), std::stoi(number, nullptr));
            // Also set each mesh's shininess property to a default value (if you want you could extend this to another mesh property and possibly change this value)
            glUniform1f(glGetUniformLocation(shader.Program,
                    "material.shininess"), 50.0f);

            // Draw mesh
            glBindVertexArray(this->VAO);
            glDrawElements(GL_TRIANGLES, this->indices.size(),
                    GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            for (GLuint i = 0; i < this->textures.size(); i++){
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
        }
    private:
        //buffers
        GLuint VAO, VBO, EBO;
        void setupMesh(){
            glGenVertexArrays(1, &this->VAO);
            glGenBuffers(1, &this->VBO);
            glGenBuffers(1, &this->EBO);

            glBindVertexArray(this->VAO);

            glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
            glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size()* sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

            //position
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0 );
            glEnableVertexAttribArray(0);

            //normal
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) offsetof(Vertex, normal) );
            glEnableVertexAttribArray(1);

            //texture
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, textureCoor) );
            glEnableVertexAttribArray(2);

            glBindVertexArray(0);
        }
};
