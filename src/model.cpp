#include "model.h"
#include "camera.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/gtx/string_cast.hpp>

Model::Model(const std::string& path, bool gamma):gamma_correction(gamma) {

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
            path.c_str(),
            aiProcess_Triangulate | 
            aiProcess_GenNormals |
            aiProcess_FlipUVs |
            aiProcess_CalcTangentSpace
            );

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error assimp " << importer.GetErrorString() << "\n";
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));
    process_node(scene->mRootNode, scene);

    position = glm::vec3(0.0f, 0.0f, -3.0f);
     stbi_set_flip_vertically_on_load(true);

    shader = new Shader("../shaders/model.vs", "../shaders/model.fs");

}


void Model::process_node(aiNode* node, const aiScene* scene) {

    for(unsigned int i = 0; i < node->mNumMeshes; i += 1) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(process_mesh(mesh, scene));
    }

    for(unsigned int i = 0; i < node->mNumChildren; i += 1)
        process_node(node->mChildren[i], scene);

}


Mesh Model::process_mesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for(unsigned int i = 0; i < mesh->mNumVertices; i += 1) {
        Vertex vertex;
        glm::vec3 vector;

        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;

        vertex.position = vector;

        if(mesh->HasNormals()) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;

            vertex.normal = vector;
        }

        if(mesh->mTextureCoords[0]) {
            glm::vec2 vec;

            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.tex_coords = vec;

            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;

            vertex.tangent = vector;

            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;

            vertex.bitangent = vector;

        }
        else
            vertex.tex_coords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);

    }

    for(unsigned int i = 0; i < mesh->mNumFaces; i += 1) {
        aiFace face = mesh->mFaces[i];

        for(unsigned int j = 0; j < face.mNumIndices; j += 1)
            indices.push_back(face.mIndices[j]);

    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    std::vector<Texture> diffuse_maps = load_material_texture(
            material,
            aiTextureType_DIFFUSE,
            "texture_diffuse"
            );

    textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

    std::vector<Texture> specular_maps = load_material_texture(
            material,
            aiTextureType_SPECULAR,
            "texture_specular"
            );

    textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());


    std::vector<Texture> normal_maps = load_material_texture(
            material,
            aiTextureType_HEIGHT,
            "texture_normal"
            );

    textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());


    std::vector<Texture> height_maps = load_material_texture(
            material,
            aiTextureType_AMBIENT,
            "texture_height"
            );

    textures.insert(textures.end(), height_maps.begin(), height_maps.end());
    return Mesh(vertices, indices, textures);

}


std::vector<Texture> Model::load_material_texture(aiMaterial* mat, aiTextureType type, std::string type_name) {
    std::vector<Texture> textures;

    for(unsigned int i = 0; i < mat->GetTextureCount(type); i += 1) {

        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;

        for(unsigned int j = 0; j < texture_loaded.size(); j += 1) {

            if(std::strcmp(texture_loaded[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(texture_loaded[i]);
                skip = true;
                break;
            }
        }

        if(!skip) {
            Texture texture;
            texture.id = texture_from_file(str.C_Str(), directory);
            texture.type = type_name;
            texture.path = str.C_Str();
            textures.push_back(texture);
            texture_loaded.push_back(texture);

        }

    }

    return textures;

}

unsigned int texture_from_file(const char* path, const std::string& directory, bool gamma) {
    std::string file_name = std::string(path);
    std::string texture_path = directory + "/" + file_name;

    std::cout << texture_path << "\n";

    unsigned int texture_id;
    glGenTextures(1, &texture_id);

    int width;
    int height;
    int nr_components;

    unsigned char* data = stbi_load(texture_path.c_str(), &width, &height, &nr_components, 0);

    if(data) {
        GLenum format;

        if(nr_components == 1)
            format = GL_RED;

        else if(nr_components == 3)
            format = GL_RGB;

        else if(nr_components == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);


    }
    else
        stbi_image_free(data);

    return 1;
}


void Model::draw() {
    for(unsigned int i = 0; i < meshes.size(); i += 1)
        meshes[i].draw(shader);
}


void Model::update() {
    shader->use();
    glm::mat4 projection = Camera::get_instance()->projection;
    glm::mat4 view = Camera::get_instance()->view;

    shader->setMat4("projection", projection);
    shader->setMat4("view", view);


    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
    shader->setMat4("model", model);

}
