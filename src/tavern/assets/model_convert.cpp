#include "converters.h"

#include <cstdio>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>
#include <assimp/material.h>
#include <assimp/scene.h>

#include <boost/log/trivial.hpp>

#include "tavern/graphics/vertex.h"
#include "tavern/graphics/material.h"

namespace tavern::assets::converter {

inline glm::vec4 conv_aicol(const aiColor4D& c) {
    return glm::vec4(c.r, c.g, c.b, c.a);
}

inline glm::vec3 conv_aivec(const aiVector3D& v) {
    return glm::vec3(v.x, v.y, v.z);
}

void export_mesh(const std::filesystem::path& out_dir, const aiScene* scene, const size_t index)
{
    const aiMesh* mesh = scene->mMeshes[index];

    std::vector<graphics::vertex> vertices;
    std::vector<graphics::face> faces;

    const size_t num_vertices = mesh->mNumVertices;
    const size_t num_faces    = mesh->mNumFaces;

    // convert to local format
    vertices.reserve(num_vertices);
    for (size_t i = 0; i < num_vertices; ++i)
    {
        graphics::vertex vertex;
        vertex.position  = conv_aivec(mesh->mVertices[i]);
        vertex.normal    = conv_aivec(mesh->mNormals[i]);
        vertex.tangent   = conv_aivec(mesh->mTangents[i]);
        vertex.bitangent = conv_aivec(mesh->mBitangents[i]);
        vertex.texcoord  = conv_aivec(mesh->mTextureCoords[0][i]);

        vertices.push_back(vertex);
    }

    faces.reserve(num_faces);
    for (size_t i = 0; i < num_faces; ++i)
    {
        graphics::face face;
        for (size_t j = 0; j < 3; ++j)
            face.indices[j] = mesh->mFaces[i].mIndices[j];

        faces.push_back(face);
    }

    // warning! non unique names may exist!
    std::filesystem::path out_file = out_dir;
    if (mesh->mName.length == 0)
        out_file /= std::to_string(index);

    else
        out_file /= mesh->mName.C_Str();
    out_file += ".tms";

    if (std::filesystem::exists(out_file))
        BOOST_LOG_TRIVIAL(warning) << out_file << " already exists, overwriting...";

    FILE* file = fopen(out_file.c_str(), "wb");
    if (!file)
        return;

    // write vertex/face counts
    fwrite(&num_vertices, sizeof(num_vertices), 1, file);
    fwrite(&num_faces   , sizeof(num_faces)   , 1, file);

    // write vertex data
    fwrite(vertices.data(), sizeof(graphics::vertex), num_vertices, file);
    
    // write face data
    fwrite(faces.data(), sizeof(graphics::face), num_faces, file);

    fclose(file);
}

// NOTE: Will always write string length, even if 0
// used to detemine if using texture or constant value
bool write_material_texture(const aiMaterial* material, aiTextureType type, FILE* file)
{
    size_t path_len = 0;
    aiString img_path;

    if (material->GetTextureCount(type)) {
        // NOTE: May not play well with file mounts
        material->GetTexture(type, 0, &img_path);
        path_len = img_path.length;
    }

    // check if file exists?

    fwrite(&path_len, sizeof(size_t), 1, file);

    if (!path_len)
        return false;

    fputs(img_path.C_Str(), file);

    return true;
}

void export_material(const std::filesystem::path& out_dir, const aiScene* scene, const size_t index)
{
    const aiMaterial* material = scene->mMaterials[index];

    std::filesystem::path out_file = out_dir;
    if (material->GetName().length == 0)
        out_file /= std::to_string(index);
    else
        out_file /= material->GetName().C_Str();
    out_file += ".tml";

    if (std::filesystem::exists(out_file))
        BOOST_LOG_TRIVIAL(warning) << out_file << " already exists, overwriting...";

    FILE* file = fopen(out_file.c_str(), "wb");
    if (!file)
        return;

    // Albedo
    if (!write_material_texture(material, aiTextureType_DIFFUSE, file))
    {
        aiColor4D ai_albedo;
        aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &ai_albedo);
        glm::vec3 albedo = conv_aicol(ai_albedo);

        fwrite(&albedo, sizeof(glm::vec3), 1, file);
    }

    // Metallic
    if (!write_material_texture(material, aiTextureType_METALNESS, file))
    {
        float metallic;
        aiGetMaterialFloat(material, AI_MATKEY_METALLIC_FACTOR, &metallic);

        fwrite(&metallic, sizeof(float), 1, file);
    }

    // Roughness
    if (!write_material_texture(material, aiTextureType_DIFFUSE_ROUGHNESS, file)) // aiTextureType_BASE_COLOR
    {
        float roughness;
        aiGetMaterialFloat(material, AI_MATKEY_ROUGHNESS_FACTOR, &roughness);

        fwrite(&roughness, sizeof(float), 1, file);
    }
    // NOTE: textures can be stored under different identifiers, should desend through list

    // Normal (No fallback constant value)
    write_material_texture(material, aiTextureType_NORMALS, file); // aiTextureType_NORMAL_CAMERA

    // Ambient Occlusion (No fallback constant value)
    write_material_texture(material, aiTextureType_LIGHTMAP, file); // aiTextureType_AMBIENT_OCCLUSION

    // Emissive
    if (!write_material_texture(material, aiTextureType_EMISSIVE, file)) // aiTextureType_EMISSION_COLOR
    {
        aiColor4D ai_emissive;
        aiGetMaterialColor(material, AI_MATKEY_COLOR_EMISSIVE, &ai_emissive);
        glm::vec3 emissive = conv_aicol(ai_emissive);

        fwrite(&emissive, sizeof(glm::vec3), 1, file);
    }


    fclose(file);
}

bool model(const std::filesystem::path& file_path)
{
    const std::filesystem::path out_dir = file_path.parent_path();

    Assimp::Importer importer;

    importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);
    importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_COLORS | aiComponent_ANIMATIONS | aiComponent_BONEWEIGHTS | aiComponent_CAMERAS);

    // speed is not of importance here, converting other formats into binary format for engine consumption
    constexpr int flags = 0
        | aiProcess_CalcTangentSpace
        | aiProcess_GenNormals
        | aiProcess_GenUVCoords
        | aiProcess_FixInfacingNormals
        | aiProcess_FindDegenerates
        | aiProcess_SortByPType
        | aiProcess_FindInvalidData
        | aiProcess_FindInstances           // NOTE: assimp won't remove identical meshes with different materials!
        | aiProcess_ImproveCacheLocality
        | aiProcess_JoinIdenticalVertices
        | aiProcess_OptimizeMeshes
        | aiProcess_OptimizeGraph
        | aiProcess_ValidateDataStructure
        | aiProcess_RemoveComponent
        | aiProcess_FlipUVs
    ;

    const aiScene* scene = importer.ReadFile(file_path.c_str(), flags);

    if (!scene) {
        BOOST_LOG_TRIVIAL(error) << "Failed to convert file: " << file_path << '\n'
            << importer.GetErrorString();
        return false;
    }

    // iterate meshes
    for (size_t i = 0; i < scene->mNumMeshes; ++i)
        export_mesh(out_dir, scene, i);

    // iterate materials
    for (size_t i = 0; i < scene->mNumMaterials; ++i)
        export_material(out_dir, scene, i);

    return true;
}

} /* end of namespace tavern::assets::converter */
