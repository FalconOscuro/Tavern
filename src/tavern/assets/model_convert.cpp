#include "converters.h"

#include <cstdio>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>

#include <boost/log/trivial.hpp>

#include "tavern/graphics/vertex.h"

namespace tavern::assets::converter {

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
        | aiProcess_RemoveComponent;

    const aiScene* scene = importer.ReadFile(file_path.c_str(), flags);

    if (!scene) {
        BOOST_LOG_TRIVIAL(error) << "Failed to convert file: " << file_path << '\n'
            << importer.GetErrorString();
        return false;
    }

    for (size_t i = 0; i < scene->mNumMeshes; ++i)
        export_mesh(out_dir, scene, i);

    return true;
}

} /* end of namespace tavern::assets::converter */
