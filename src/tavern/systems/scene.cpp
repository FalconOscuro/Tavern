#include "tavern/systems/scene.h"

#include <cassert>
#include <unordered_map>

#include <boost/log/trivial.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

#include <ryml.hpp>

#include "tavern/components/drawable3d.h"
#include "tavern/components/transform3d.h"
#include "tavern/components/camera.h"

#include "tavern/graphics/material.h"
#include "tavern/resource/resource_manager.h"
#include "tavern/graphics/vertex.h"
#include "tavern/graphics/mesh.h"
#include "tavern/resource/util/file.hpp"

template <>
struct std::hash<c4::csubstr>
{
    std::size_t operator()(const c4::csubstr& s) const {
        const std::string_view sv = std::string_view(s.data(), s.size());

        return std::hash<std::string_view>()(sv);
    }
}; /* end of struct std::hash<c4::csubstr> */

namespace tavern::system {

void scene::update(ecs::registry& reg)
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

[[nodiscard]] glm::mat4 convert_matrix(const aiMatrix4x4& m)
{
    return glm::mat4(
        m.a1, m.b1, m.c1, m.d1,
        m.a2, m.b2, m.c2, m.d2,
        m.a3, m.b3, m.c3, m.d3,
        m.a4, m.b4, m.c4, m.d4
    );
}

void load_node(aiNode* node, const aiScene* scene, ecs::registry& reg, ecs::entity_type parent, const std::string& path)
{
    component::transform t = component::transform(convert_matrix(node->mTransformation), parent); // temporary hack to oreient model

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

void scene::load(const std::string& file, ecs::registry& reg)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        BOOST_LOG_TRIVIAL(error) << "Failed to load scene from file: " << file;
        return;
    }

    load_node(scene->mRootNode, scene, reg, reg.tombstone(), file);
}

// Note, could be built into ECS
void scene::load_scene(const std::string& file, ecs::registry& reg)
{
    BOOST_LOG_TRIVIAL(info) << "Loading scene from " << file;

    size_t size;
    char* raw = resource::utility::read_file(file.c_str(), size);

    if (raw == nullptr) 
        return;

    const ryml::Tree tree = ryml::parse_in_place(raw);
    const ryml::ConstNodeRef root = tree.crootref();

    if (!root.is_map()) {
        BOOST_LOG_TRIVIAL(error) << "Improper format for scene file, root node must be map of entities!";
        delete[] raw;
        return;
    }

    std::unordered_map<c4::csubstr, ecs::entity_type> loaded_entity_map;

    // create ids for all entities for smooth transition
    for (auto entity = root.begin(); entity != root.end(); ++entity)
    {
        const c4::csubstr key = (*entity).key();

        if (loaded_entity_map.count(key)) {
            BOOST_LOG_TRIVIAL(error) << "Found duplicate key \"" << key << "\" whilst loading scene from " << file;
            continue;
        }

        loaded_entity_map.emplace(std::make_pair(key, reg.create()));
    }

    // iterate all entities now created and attatch components
    for (auto entity = loaded_entity_map.cbegin(); entity != loaded_entity_map.end(); ++entity)
    {
        const ecs::entity_type eid = entity->second;
        const ryml::ConstNodeRef node = root[entity->first];

        // slow string comparison of individual component types, implementation could be nicer
        // Factory?
        if (node.has_child("camera")) {
            node["camera"] >> reg.emplace<component::camera>(eid);
        }

        if (node.has_child("transform3d")) {
        }

        if (node.has_child("drawable3d")) {
        }
    }

    // NOTE: need way of referring to loaded entity map for objects such as transforms which have reference to other entity id which at current is local to the serialized version

    delete[] raw;
    BOOST_LOG_TRIVIAL(info) << "Scene loaded";
}

} /* end of namespace tavern::system */
