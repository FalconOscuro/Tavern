#include "tavern/systems/scene_tree.h"

#include <cassert>

#include <boost/log/trivial.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

#include "tavern/components/drawable3d.h"
#include "tavern/components/transform3d.h"
#include "tavern/graphics/material.h"
#include "tavern/resource/resource_manager.h"
#include "tavern/graphics/vertex.h"
#include "tavern/graphics/mesh.h"
#include "tavern/resource/util/file.hpp"

namespace tavern::system {

void scene_tree::update(ecs::registry& reg)
{
    auto& pool = reg.get_pool<component::transform>();

    // add untracked entities
    for (auto it = pool.begin(); it != pool.end(); ++it) {
        m_entities.emplace(it->second);
    }

    // sort entities and remove deleted
    {
        auto it = m_entities.begin();

        while (it != m_entities.end())
        {
            if (!pool.exists(*it)) {
                it = m_entities.remove_inplace(it);
                continue;
            }

            component::transform& t = pool.get(*it);

            // is root node, global = local
            if (t.parent >= pool.tombstone() || !pool.exists(t.parent)) {
                t.parent = pool.tombstone();
                t.m_global = t.local;
            }

            // NOTE: need to avoid circular relationships
            // check index for parent, swap if greater
            else if (m_entities.find(t.parent) > m_entities.find(*it)) {
                m_entities.swap(t.parent, *it);
                continue;
            }

            // if index is lower, parent already processed, safe to calculate global
            else
                t.m_global = pool.get(t.parent).m_global * t.local;

            ++it;
        }
    }
}

[[nodiscard]] std::shared_ptr<graphics::texture2d> load_texture(aiMaterial* material, aiTextureType type, const std::string& dir)
{
    if (material->GetTextureCount(type) == 0)
        return nullptr;

    aiString img_path;
    material->GetTexture(type, 0, &img_path);

    return resource_manager::get().tex2ds.load(dir + std::string(img_path.C_Str()));
}

[[nodiscard]] graphics::material load_material(aiMaterial* material, const std::string& dir)
{
    graphics::material mat;

    // TODO: Read non texture properties

    mat.albedo_tex = load_texture(material, aiTextureType_DIFFUSE, dir);
    mat.metallic_roughness_tex = load_texture(material, aiTextureType_UNKNOWN, dir);
    mat.normal_tex = load_texture(material, aiTextureType_NORMALS, dir);
    mat.ambient_occlusion_tex = load_texture(material, aiTextureType_LIGHTMAP, dir);
    mat.emissive_tex = load_texture(material, aiTextureType_EMISSIVE, dir);

    return mat;
}

[[nodiscard]] std::shared_ptr<graphics::mesh> load_mesh(uint32_t mesh_id, const aiScene* scene, const std::string& path)
{
    const std::string mesh_name = path + ";mesh:" + std::to_string(mesh_id);
    auto & resource_mgr = resource_manager::get();

    if (resource_mgr.meshes.is_loaded(mesh_name))
        return resource_mgr.meshes.load(mesh_name);

    aiMesh* mesh = scene->mMeshes[mesh_id];

    std::vector<graphics::vertex> vertices;
    std::vector<uint32_t> indices;

    vertices.reserve(mesh->mNumVertices);
    indices.reserve(mesh->mNumFaces * 3);

    // vertices
    for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
    {
        graphics::vertex v;

        // NOTE: Loaded models seem to be flipped in x/y?
        v.position   = glm::vec3(mesh->mVertices[i].x  , mesh->mVertices[i].y  , mesh->mVertices[i].z  );
        v.normal     = glm::vec3(mesh->mNormals[i].x   , mesh->mNormals[i].y   , mesh->mNormals[i].z   );
        v.tangent    = glm::vec3(mesh->mTangents[i].x  , mesh->mTangents[i].y  , mesh->mTangents[i].z  );
        v.bi_tangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);

        if (mesh->mTextureCoords[0])
            v.texture_coordinates = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);

        vertices.push_back(v);
    }

    // indices
    for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace& face = mesh->mFaces[i];

        // assert here for loop unrolling below
        assert(face.mNumIndices == 3 && "Expected triangulated mesh but found face where indicies != 0");

        for (uint32_t j = 0; j < 3; ++j)
            indices.push_back(face.mIndices[j]);
    }

    std::shared_ptr<graphics::material> material_ptr;
    std::string material_name = path + ";material:" + std::to_string(mesh->mMaterialIndex);
    
    if (resource_mgr.materials.is_loaded(material_name))
        material_ptr = resource_mgr.materials.load(material_name);

    else {
        graphics::material material = load_material(scene->mMaterials[mesh->mMaterialIndex], std::string(resource::utility::get_file_parent_dir(path)));
        material_ptr = resource_mgr.materials.register_new(material_name, material);
    }

    return resource_mgr.meshes.register_new(mesh_name, vertices, indices, material_ptr);
}

void load_node(aiNode* node, const aiScene* scene, ecs::registry& reg, ecs::entity_type parent, const std::string& path)
{
    // This can't be safe right?
    // *Theoretically* both should have the same structure in memory
    component::transform t = component::transform(*((glm::mat4*)&node->mTransformation), parent);

    if (node->mNumMeshes > 1) {
        ecs::entity_type eid = reg.create();
        reg.set(eid, t);
        parent = eid;

        t = component::transform();
        t.parent = parent;
    }

    // iterate all meshes on node
    for (uint64_t i = 0; i < node->mNumMeshes; ++i) {
        ecs::entity_type eid = reg.create();
        reg.set(eid, t);

        std::shared_ptr<graphics::mesh> mesh = load_mesh(node->mMeshes[i], scene, path);

        reg.emplace<component::drawable3d>(eid, mesh);

        if (i == 0 && node->mNumMeshes == 1)
            parent = eid;
    }

    // iterate over children
    for (uint64_t i = 0; i < node->mNumChildren; ++i)
        load_node(node->mChildren[i], scene, reg, parent, path);
}

void scene_tree::load_scene(const std::string& file, ecs::registry& reg)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        BOOST_LOG_TRIVIAL(error) << "Failed to load scene from file: " << file;
        return;
    }

    load_node(scene->mRootNode, scene, reg, reg.tombstone(), file);
}

} /* end of namespace tavern::system */
