#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include "shader.h"
#include "mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


unsigned int texture_from_file(const char* path, const std::string& directory,bool gamma=false);

struct Model {
    Model(
            const std::string& path,
            const std::string& vertex_shader_path,
            const std::string& fragment_shader_path,
            bool has_texture=true, bool gamma=false);
    void draw();
    void update();
    void process_node(aiNode* node, const aiScene* scene);
    Mesh process_mesh(aiMesh* mesh, const aiScene* scene);

    std::vector<Mesh> meshes;
    std::vector<Texture> texture_loaded;
    std::vector<Texture> load_material_texture(aiMaterial* mat, aiTextureType type, std::string type_name);
    std::string directory;

    bool gamma_correction;

    Shader* shader;
    glm::vec3 position;



    bool has_texture;



};


#endif
