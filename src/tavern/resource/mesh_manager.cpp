#include "tavern/resource/mesh_manager.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <boost/log/trivial.hpp>

#include "assimp/postprocess.h"
#include "assimp/vector3.h"
#include "tavern/resource/util/assimp_io.h"

namespace tavern::resource {

inline glm::vec4 glm_convert(const aiColor4D& c) {
    return glm::vec4(c.r, c.g, c.b, c.a);
}

inline glm::vec3 glm_convert(const aiVector3D& v) {
    return glm::vec3(v.x, v.y, v.z);
}

graphics::mesh* mesh_manager::load_new(file::ifile* file)
{
    // must be a better way of doing this
    file->close();

    Assimp::Importer imp;
    imp.SetIOHandler(new util::assimp_iosystem());

    const int flags = 0
        | aiProcess_CalcTangentSpace
        | aiProcess_GenNormals
        | aiProcess_GenUVCoords
        | aiProcess_Triangulate
        | aiProcess_JoinIdenticalVertices
        ;

    const aiScene* scene = imp.ReadFile(file->get_path().get_full().data(), flags);

    if (!scene) {
        BOOST_LOG_TRIVIAL(error) << "Failed to load mesh '" << file->get_path() << "': " << imp.GetErrorString();
        return nullptr;
    }

    else if (!(scene->HasMeshes() && scene->mRootNode->mNumMeshes)) {
        BOOST_LOG_TRIVIAL(error) << "Failed loading mesh '" << file->get_path() << "': No meshes found in file";
        return nullptr;
    }

    const aiMesh* mesh = scene->mMeshes[scene->mRootNode->mMeshes[0]];

    std::vector<uint32_t> indices;
    indices.reserve(mesh->mNumFaces * 3);

    for (size_t i = 0; i < mesh->mNumFaces; ++i)
        for (size_t j = 0; j < 3; ++j)
            indices.push_back(mesh->mFaces[i].mIndices[j]);

    std::vector<graphics::vertex> vertices;
    vertices.reserve(mesh->mNumVertices);

    for (size_t i = 0; i < mesh->mNumVertices; ++i)
    {
        graphics::vertex v;
        v.position = glm_convert(mesh->mVertices[i]);
        v.normal = glm_convert(mesh->mNormals[i]);
        v.tangent = glm_convert(mesh->mTangents[i]);
        v.bi_tangent = glm_convert(mesh->mBitangents[i]);
        v.texture_coordinates = glm_convert(mesh->mTextureCoords[0][i]);
        v.colour = glm_convert(mesh->mColors[0][i]);

        vertices.push_back(v);
    }

    return new graphics::mesh(vertices, indices);
}

} /* end of namespace tavern::resource */
