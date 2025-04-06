#include "tavern/resource/material_manager.h"

#include <boost/log/trivial.hpp>

#include "tavern/core/resource_manager.h"

namespace tavern::resource {

file::mount_path resolve_path(std::string_view path, const file::ifile* parent_file)
{
    file::mount_path mount;

    // absolute path
    if (path.find(':') != path.npos)
        file::mount_path::try_create(path, mount);

    else
    {
        const std::string_view parent_dir = parent_file->get_directory();

        std::string relative_path;
        relative_path.reserve(parent_dir.length() + 1 + path.length());

        relative_path.append(parent_dir);
        relative_path.push_back('/');
        relative_path.append(path);

        mount = file::mount_path(parent_file->get_path().get_identifier(), relative_path);
    }

    return mount;
}

bool try_read_attr_str(std::string& path, file::ifile* file)
{
    size_t len;
    file::read_data(&len, file);
    path.clear();


    if (!len || file->eof())
        return false;

    path.reserve(len + 1);
    
    // ifile::getstr requires temporary buffer to write to std::string, instead just appending chars to str
    for (size_t i = 0; i < len && !file->eof(); ++i)
        path.push_back(file->get_char());

    return len == path.length();
}

graphics::material* material_manager::load_new(file::ifile* file)
{
    graphics::material material;
    std::string path_str = "";

    auto& tex_mgr = resource_manager::singleton().tex2ds;

    // WARNING: Potential issues when encountering file end unexpectedly!

    // Albedo
    if (try_read_attr_str(path_str, file)) {
        // Using texture
        const file::mount_path path = resolve_path(path_str, file);
        material.albedo_tex = tex_mgr.load(path);
        // Post warning here?
    }
    else
        file::read_data(&material.albedo, file);

    // Metallic
    if (try_read_attr_str(path_str, file)) {
        // Using texture
        const file::mount_path path = resolve_path(path_str, file);
        material.metallic_tex = tex_mgr.load(path);
    }
    else
        file::read_data(&material.metallic, file);

    // Roughness
    if (try_read_attr_str(path_str, file)) {
        // Using texture
        const file::mount_path path = resolve_path(path_str, file);
        material.roughness_tex = tex_mgr.load(path);
    }
    else
        file::read_data(&material.roughness, file);

    // Normal
    if (try_read_attr_str(path_str, file)) {
        // Using texture
        const file::mount_path path = resolve_path(path_str, file);
        material.normal_tex = tex_mgr.load(path);
    }
    // no fallback
    
    // Ambient Occlusion
    if (try_read_attr_str(path_str, file)) {
        // Using texture
        const file::mount_path path = resolve_path(path_str, file);
        material.ambient_occlusion_tex = tex_mgr.load(path);
    }
    // no fallback

    // Emissive
    if (try_read_attr_str(path_str, file)) {
        // Using texture
        const file::mount_path path = resolve_path(path_str, file);
        material.emissive_tex = tex_mgr.load(path);
    }
    else
        file::read_data(&material.emissive, file);

    return new graphics::material(material);
}

} /* end of namespace tavern::resource */
