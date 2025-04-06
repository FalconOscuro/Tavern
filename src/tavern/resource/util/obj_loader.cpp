#include "tavern/resource/util/obj_loader.h"

#include <charconv>

#include <boost/log/trivial.hpp>
#include <glm/geometric.hpp>

#include "tavern/core/resource_manager.h"

#define CHECK_TOKEN(s, t) strncmp(s, t, strlen(t)) == 0

#define OBJ_TOKEN_COMMENT  '#'
#define OBJ_TOKEN_VERTEX   "v "
#define OBJ_TOKEN_NORMAL   "vn "
#define OBJ_TOKEN_TEXTURE  "vt "
#define OBJ_TOKEN_FACE     "f "

#define OBJ_TOKEN_OBJECT   "o "
#define OBJ_TOKEN_GROUP    "g "
#define OBJ_TOKEN_LINE     "l "
#define OBJ_TOKEN_SMOOTH   "s "
#define OBJ_TOKEN_MTLLIB   "mtllib "
#define OBJ_TOKEN_MTLUSE   "usemtl "

#define MTL_TOKEN_NEWMTL   "newmtl "
#define MTL_TOKEN_AMBIENT  "Ka "
#define MTL_TOKEN_DIFFUSE  "Kd "
#define MTL_TOKEN_SPECULAR "Ns "
#define MTL_TOKEN_EMISSIVE "Ke "
#define MTL_TOKEN_MAP      "map_"

namespace tavern::resource::util {

struct obj_face_element
{
    size_t vertex = size_t(-1);
    size_t tex_coord = size_t(-1);
    size_t normal = size_t(-1);
}; /* end of struct obj_face_element */

struct obj_face
{
    obj_face_element indices[3];
}; /* end of struct obj_face */

inline void read_line(file::ifile* file, std::string& line_buf) {
    line_buf.clear();

    for (char c = file->get_char(); !file->eof() && c != '\n'; c = file->get_char())
        line_buf.push_back(c);
}

template<typename T>
bool strv_to_num(const std::string_view& s, T& n) {
    return std::from_chars(s.data(), s.data() + s.length(), n).ec == std::errc();
}

std::string_view trim_ws(const std::string_view& s)
{
    const size_t start = s.find_first_not_of(" \t");
    const size_t end = s.find_last_not_of(" \t");

    return s.substr(start, end - start);
}

bool read_vec2(std::string_view line, glm::vec2& v)
{
    line = trim_ws(line);

    const size_t split = line.find_first_of(' ');

    // failed to find split
    if (split == line.npos)
        return false;

    const std::string_view v0 = line.substr(0, split);
    const std::string_view v1 = line.substr(split + 1);

    // too many split points
    if (v1.find(' ') != v1.npos)
        return false;

    return strv_to_num(v0, v.x) && strv_to_num(v1, v.y);
}

bool read_vec3(std::string_view line, glm::vec3& v)
{
    line = trim_ws(line);

    const size_t split_1 = line.find_first_of(' ');
    const size_t split_2 = line.find_last_of(' ');

    // could not find 2 split points
    if (split_1 == line.npos || split_2 == line.npos || split_1 == split_2)
        return false;

    const std::string_view v0 = line.substr(0, split_1);
    const std::string_view v1 = line.substr(split_1 + 1, split_2 - (split_1 + 1));
    const std::string_view v2 = line.substr(split_2 + 1);

    // more than 2 split points
    if (v1.find(' ') != v1.npos)
        return false;

    return strv_to_num(v0, v.x) && strv_to_num(v1, v.y) && strv_to_num(v2, v.z);
}

bool read_obj_face_element(std::string_view line, obj_face_element& element)
{
    const size_t split_1 = line.find_first_of('/');
    const size_t split_2 = line.find_last_of('/');

    std::string_view v, t, n;

    // vertex only
    if (split_1 == line.npos)
        v = line;

    // vertex and tex_coord
    else if (split_1 == split_2)
    {
        v = line.substr(0, split_1);
        t = line.substr(split_1 + 1);
    }

    // all 3
    else
    {
        v = line.substr(0, split_1);
        t = line.substr(split_1 + 1, split_2 - (split_1 + 1));
        n = line.substr(split_2 + 1);
    }

    // vertex & normal are required
    if (v.empty() || n.empty() || t.empty())
        return false;

    bool success = strv_to_num(v, element.vertex) && strv_to_num(n, element.normal);

    if (!t.empty()) {
        success &= strv_to_num(t, element.tex_coord);
        element.tex_coord -= 1;
    }

    element.vertex -= 1;
    element.normal -= 1;

    return success;
}

bool read_obj_face(std::string_view line, obj_face& face)
{
    line = trim_ws(line);

    const size_t split_1 = line.find_first_of(' ');
    const size_t split_2 = line.find_last_of(' ');

    // could not find 2 split points
    if (split_1 == line.npos || split_2 == line.npos || split_1 == split_2)
        return false;

    const std::string_view v0 = line.substr(0, split_1);
    const std::string_view v1 = line.substr(split_1 + 1, split_2 - (split_1 + 1));
    const std::string_view v2 = line.substr(split_2 + 1);

    // more than 2 split points
    if (v1.find(' ') != v1.npos) {
        BOOST_LOG_TRIVIAL(error) << "Only triangulated meshes are currently supported!";
        return false;
    }

    return read_obj_face_element(v0, face.indices[0]) && read_obj_face_element(v1, face.indices[1]) && read_obj_face_element(v2, face.indices[2]);
}

bool load_obj(file::ifile* file, std::vector<graphics::vertex>& vertices, std::vector<graphics::face>& faces)
{
    faces.clear();
    vertices.clear();

    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> tex_coords;
    std::vector<obj_face> obj_faces;

    // failed to open file
    if (!file->open()) {
        BOOST_LOG_TRIVIAL(error) << "Obj loader failed to open file: " << file->get_path();
        return false;
    }
    // line stored in std::string for resizeable buffer
    std::string line;

    // read data
    for (size_t line_no = 0; !file->eof(); ++line_no)
    {
        read_line(file, line);

        std::string_view line_sv = line;
        const char* line_cstr = line.c_str();

        // empty line or comment
        if (line_sv.empty() || line[0] == OBJ_TOKEN_COMMENT || line[0] == '\0') {
            continue;
        }

        // vertex
        else if (CHECK_TOKEN(line_cstr, OBJ_TOKEN_VERTEX))
        {
            line_sv.remove_prefix(strlen(OBJ_TOKEN_VERTEX));
            graphics::vertex v;

            if (!read_vec3(line_sv, v.position)) {
                BOOST_LOG_TRIVIAL(error) << "Failed to parse vertex on line: " << line_no;
                file->close();
                return false;
            }

            vertices.push_back(v);
        }

        // normal
        else if (CHECK_TOKEN(line_cstr, OBJ_TOKEN_NORMAL))
        {
            line_sv.remove_prefix(strlen(OBJ_TOKEN_NORMAL));
            glm::vec3 normal;

            if (!read_vec3(line_sv, normal)) {
                BOOST_LOG_TRIVIAL(error) << "Failed to parse normal on line: " << line_no;
                file->close();
                return false;
            }

            normals.push_back(normal);
        }

        // texture
        else if (CHECK_TOKEN(line_cstr, OBJ_TOKEN_TEXTURE))
        {
            line_sv.remove_prefix(strlen(OBJ_TOKEN_TEXTURE));
            glm::vec2 tex_coord;

            if (!read_vec2(line_sv, tex_coord)) {
                BOOST_LOG_TRIVIAL(error) << "Failed to parse tex coord on line: " << line_no;
                file->close();
                return false;
            }

            tex_coords.push_back(tex_coord);
        }

        else if (CHECK_TOKEN(line_cstr, OBJ_TOKEN_FACE))
        {
            line_sv.remove_prefix(strlen(OBJ_TOKEN_FACE));
            obj_face face;

            if (!read_obj_face(line_sv, face)) {
                BOOST_LOG_TRIVIAL(error) << "Failed to parse face on line: " << line_no;
                file->close();
                return false;
            }

            obj_faces.push_back(face);
        }

        // UNSUPPORTED SYMBOLS

        // line
        else if (CHECK_TOKEN(line_cstr, OBJ_TOKEN_LINE))
            BOOST_LOG_TRIVIAL(warning) << "line:" << line_no << ", file:" << file->get_path()
                << ", OBJ polylines are unsupported";

        // object
        else if (CHECK_TOKEN(line_cstr, OBJ_TOKEN_OBJECT))
            BOOST_LOG_TRIVIAL(warning) << "line:" << line_no << ", file:" << file->get_path()
                << ", OBJ named objects are unsupported";

        // group
        else if (CHECK_TOKEN(line_cstr, OBJ_TOKEN_GROUP))
            BOOST_LOG_TRIVIAL(warning) << "line:" << line_no << ", file:" << file->get_path()
                << ", OBJ face groups are unsupported";

        // smooth shading
        else if (CHECK_TOKEN(line_cstr, OBJ_TOKEN_SMOOTH))
            BOOST_LOG_TRIVIAL(warning) << "line:" << line_no << ", file:" << file->get_path()
                << ", OBJ smooth shading is unsupported";

        // materials
        else if (CHECK_TOKEN(line_cstr, OBJ_TOKEN_MTLLIB) || CHECK_TOKEN(line_cstr, OBJ_TOKEN_MTLUSE))
            BOOST_LOG_TRIVIAL(warning) << "line:" << line_no << ", file:" << file->get_path()
                << ", OBJ mtllib is unsupported";

        // unrecognized
        else
            BOOST_LOG_TRIVIAL(warning) << "line:" << line_no << ", file:" << file->get_path()
                << ", Unrecognized token: " << line;
    }

    file->close();
    faces.reserve(obj_faces.size());

    // apply normals and tex_coords & check face indices are in range
    for (size_t i = 0; i < obj_faces.size(); ++i)
    {
        const obj_face& o_face = obj_faces[i];
        graphics::face face;

        // iterate points on face
        for (size_t j = 0; j < 3; ++j)
        {
            const obj_face_element& element = o_face.indices[j];

            if (element.vertex >= vertices.size()) {
                BOOST_LOG_TRIVIAL(error) << "Vertex index: " << element.vertex << ", for face: " << i
                    << ", is out of range!";
                return false;
            }

            else if (element.normal >= normals.size()) {
                BOOST_LOG_TRIVIAL(error) << "Normal index: " << element.normal << ", for face: " << i
                    << ", is out of range!";
                return false;
            }

            graphics::vertex& vertex = vertices[element.vertex];

            vertex.normal = normals[element.normal];
            if (element.tex_coord <= tex_coords.size())
                vertex.texcoord = tex_coords[element.tex_coord];

            face.indices[j] = element.vertex;
        }

        // tangent calculation
        const graphics::vertex& p0 = vertices[face.indices[0]];
        const graphics::vertex& p1 = vertices[face.indices[1]];
        const graphics::vertex& p2 = vertices[face.indices[2]];

        // position differences
        const glm::vec3 v = p1.position - p0.position;
        const glm::vec3 w = p2.position - p0.position;

        // texture offset for uv direction
        glm::vec2 s = p1.texcoord - p0.texcoord;
        glm::vec2 t = p2.texcoord - p0.texcoord;
        const float dir_correction = (t.x * s.y - t.y * s.x) < 0.0f ? -1.0f : 1.0f;
        // default UV direction if all occupying same position in UV space
        if (t.x * s.y == t.y * s.x)
        {
            s = glm::vec2(0.0f, 1.0f);
            t = glm::vec2(1.0f, 0.0f);
        }

        const glm::vec3 tangent   = ((w * s.y) - (v * t.y)) * dir_correction;
        const glm::vec3 bitangent = ((w * s.x) - (v * t.x)) * dir_correction;

        // apply tangent to points
        for (size_t j = 0; j < 3; ++j)
        {
            graphics::vertex& p = vertices[face.indices[j]];

            glm::vec3 local_tangent   = glm::normalize(tangent - p.normal * (tangent * p.normal));
            glm::vec3 local_bitangent = glm::normalize(bitangent - p.normal * (bitangent * p.normal));

            // NaN or INF
            const bool invalid_tangent   = glm::any(glm::isnan(local_tangent) || glm::isinf(local_tangent));
            const bool invalid_bitangent = glm::any(glm::isnan(local_bitangent) || glm::isinf(local_bitangent));

            // attempt reconstruct
            if (invalid_tangent != invalid_bitangent)
            {
                if (invalid_tangent)
                    local_tangent = glm::normalize(glm::cross(p.normal, local_bitangent));

                else
                    local_bitangent = glm::normalize(glm::cross(p.normal, local_tangent));
            }

            p.tangent = local_tangent;
            p.bitangent = local_bitangent;
        }

        faces.push_back(face);
    }


    return true;
}

file::mount_path resolve_path(std::string_view path, const file::ifile* parent_file)
{
    path = trim_ws(path);
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

bool load_mtl(file::ifile* file, tavern::graphics::material& material)
{
    if (!file->open())
        return false;

    std::string line;
    bool mtl_start_found = false;

    for (size_t line_no = 0; !file->eof(); ++line_no)
    {
        read_line(file, line);
        const char* line_cstr = line.c_str();
         std::string_view line_sv = line;

        // comment/empty line
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // material
        else if (CHECK_TOKEN(line_cstr, MTL_TOKEN_NEWMTL))
        {
            if (mtl_start_found) {
                BOOST_LOG_TRIVIAL(warning) << "Only one material supported per file!";
                break;
            }

            mtl_start_found = true;
        }

        else if (CHECK_TOKEN(line_cstr, MTL_TOKEN_DIFFUSE))
        {
            line_sv.remove_prefix(strlen(MTL_TOKEN_DIFFUSE));
            if (!read_vec3(line_sv, material.albedo))
                BOOST_LOG_TRIVIAL(warning) << "Failed to read albedo on line: " << line_no;
        }

        else if (CHECK_TOKEN(line_cstr, MTL_TOKEN_EMISSIVE))
        {
            line_sv.remove_prefix(strlen(MTL_TOKEN_EMISSIVE));
            if (!read_vec3(line_sv, material.emissive))
                BOOST_LOG_TRIVIAL(warning) << "Failed to read emissive on line: " << line_no;
        }

        else if (CHECK_TOKEN(line_cstr, MTL_TOKEN_MAP MTL_TOKEN_DIFFUSE))
        {
            line_sv.remove_prefix(strlen(MTL_TOKEN_MAP MTL_TOKEN_DIFFUSE));

            const file::mount_path path = resolve_path(line_sv, file);

            graphics::texture2d_resource tex = resource_manager::singleton().tex2ds.load(path);

            material.albedo_tex = tex;
        }

        else {
            BOOST_LOG_TRIVIAL(warning) << "Unrecognized symbol on line: " << line_no;
        }
    }
    file->close();

    return true;
}

} /* end of namespace tavern::resource::util */
