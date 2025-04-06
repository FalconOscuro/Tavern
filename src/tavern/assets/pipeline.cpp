#include "tavern/assets/pipeline.h"

#include <filesystem>
#include <unordered_map>

#include <boost/log/trivial.hpp>

#include "converters.h"

namespace tavern::assets {

enum file_type {
    UNKNOWN,
    MODEL,
    IMAGE, // does not get converted
    MATERIAL // unsupported
}; /* end of enum file_type */

const std::unordered_map<std::string, file_type> FILE_EXTENSION_MAP = {
    // All image conversions handled by stb_image
    {"jpg" , IMAGE},
    {"png" , IMAGE},
    {"tga" , IMAGE},
    {"bmp" , IMAGE},
    {"psd" , IMAGE},
    {"gif" , IMAGE},
    {"hdr" , IMAGE},
    {"pic" , IMAGE},
    {"pnm" , IMAGE},
    // 3d asset files handled by assimp
    {"3d"  , MODEL},
    {"3ds" , MODEL},
    {"3mf" , MODEL},
    {"ac"  , MODEL},
    {"ac3d", MODEL},
    {"acc" , MODEL},
    {"amj" , MODEL},
    {"ase" , MODEL},
    {"ask" , MODEL},
    {"b3d" , MODEL},
    {"bvh" , MODEL},
    {"csm" , MODEL},
    {"cob" , MODEL},
    {"dae" , MODEL},
    {"dxf" , MODEL},
    {"enff", MODEL},
    {"fbx" , MODEL},
    {"gltf", MODEL},
    {"glb" , MODEL},
    {"iqm" , MODEL},
    {"irr" , MODEL},
    {"lwo" , MODEL},
    {"lws" , MODEL},
    {"lxo" , MODEL},
    {"m3d" , MODEL},
    {"md2" , MODEL},
    {"md3" , MODEL},
    {"md5" , MODEL},
    {"mdc" , MODEL},
    {"mdl" , MODEL},
    {"obj" , MODEL} // there are more supported file formats by assimp, may add later
};

bool convert_file(const std::string& file_name)
{
    const std::filesystem::path path = file_name;

    if (!(std::filesystem::exists(path) && std::filesystem::is_regular_file(path))) {
        BOOST_LOG_TRIVIAL(error) << "Failed to find file: " << path;
        return false;
    }

    std::string extension = path.extension();
    if (extension.size() <= 1) {
        BOOST_LOG_TRIVIAL(error) << "Could not find extension for: " << path;
        return false;
    }

    extension = extension.substr(1);
    const auto found = FILE_EXTENSION_MAP.find(extension);
    file_type type = UNKNOWN;

    if (found != FILE_EXTENSION_MAP.end())
        type = found->second;

    BOOST_LOG_TRIVIAL(trace) << "Detected as filetype: " << type;

    switch (type)
    {
    case IMAGE:
        return true;

    case MATERIAL:
        return false;

    case MODEL:
        return converter::model(path);

    default:
        return false;
    }
}

} /* end of namespace tavern::assets */
