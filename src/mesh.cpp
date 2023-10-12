#include "mesh.h"


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures):
    vertices(vertices),
    indices(indices),
    textures(textures) {
        init();
        texture_active = true;
}


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
            const glm::vec4& diffuse, const glm::vec4& specular):
    vertices(vertices),
    indices(indices),
    diffuse(diffuse),
    specular(specular)
     {
         init();
         texture_active = false;

}

void Mesh::init() {

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(
                GL_ARRAY_BUFFER,
                vertices.size() * sizeof(Vertex),
                &vertices[0],
                GL_STATIC_DRAW
                );

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        glBufferData(
                GL_ELEMENT_ARRAY_BUFFER,
                indices.size() * sizeof(unsigned int),
                &indices[0],
                GL_STATIC_DRAW
                );

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
                0,
                3,
                GL_FLOAT,
                GL_FALSE,
                sizeof(Vertex),
                (void*)0
                );

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
                1,
                3,
                GL_FLOAT,
                GL_FALSE,
                sizeof(Vertex),
                (void*) offsetof(Vertex, normal)
                );


        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
                2,
                2,
                GL_FLOAT,
                GL_FALSE,
                sizeof(Vertex),
                (void*) offsetof(Vertex, tex_coords)
                );


        glEnableVertexAttribArray(3);
        glVertexAttribPointer(
                3,
                3,
                GL_FLOAT,
                GL_FALSE,
                sizeof(Vertex),
                (void*) offsetof(Vertex, tangent)
                );


        glEnableVertexAttribArray(4);
        glVertexAttribPointer(
                4,
                3,
                GL_FLOAT,
                GL_FALSE,
                sizeof(Vertex),
                (void*) offsetof(Vertex, bitangent)
                );



        glBindVertexArray(0);

}


void Mesh::draw(Shader* shader) {

    unsigned int diffuse_nr = 1;
    unsigned int specular_nr = 1;
    unsigned int normal_nr = 1;
    unsigned int height_nr = 1;

    if(texture_active) {

        for(unsigned int i = 0; i < textures.size(); i += 1) {

            std::string number;
            std::string name = textures[i].type;

            if(name == "texture_diffuse") {
                number = std::to_string(diffuse_nr);
                diffuse_nr += 1;
            }

            else if(name == "texture_specular") {
                number = std::to_string(specular_nr);
                specular_nr += 1;
            }

            std::string color_name = name + number;
            shader->setInt(color_name, i);

            //std::cout << color_name << "\n";


            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        glActiveTexture(GL_TEXTURE0);

    } else {
        shader->setVec4("material.diffuse", diffuse);
        shader->setVec4("material.specular", specular);
    }



    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);



}
