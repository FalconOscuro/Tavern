#include "file/tpk/tpk_package.h"

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <vector>

#include <boost/log/trivial.hpp>

#include "file/path.h"
#include "file/tpk/tpk.h"

namespace fs = std::filesystem;

namespace tavern::file::tpk {

template <typename T>
inline bool write_data(std::FILE* out, const T* const data, size_t count = 1)
{
    bool success = true;

    // not checking for success until end for loop unrolling
    for (size_t i = 0; i < sizeof(T) * count; ++i)
        success &= fputc(((const char*)data)[i], out) != EOF;

    return success;
}

inline size_t copy_file_contents(std::FILE* dest, std::FILE* src)
{
    size_t size = 0;

    for (char c = fgetc(src); !(c == EOF && feof(src)); c = fgetc(src))
    { 
        fputc(c, dest);
        ++size;
    }

    return size;
}

// TODO: Defer writing data into TPK to avoid tempfile
size_t recurse_parse_directory(std::FILE* out_file, std::FILE* temp_file, const fs::path& dir, size_t index = 0)
{
    std::vector<directory_entry> entries;

    for (const auto& dir_entry : fs::directory_iterator{dir})
    {
        const fs::path& entry_path = dir_entry;
        const std::string file_name = entry_path.filename();

        if (file_name.length() > directory_entry::MAX_NAME_LEN) {
            BOOST_LOG_TRIVIAL(warning) << "Skipping '" << entry_path << '\''
                << ", name_length(" << file_name.length() << ')'
                << ", filename exceeds max length " << directory_entry::MAX_NAME_LEN;
            continue;
        }

        directory_entry entry;
        entry.name_len = file_name.length();
        memcpy(entry.name, file_name.c_str(), std::min(directory_entry::MAX_NAME_LEN, file_name.length()));

        if (fs::is_empty(dir_entry)) {
            BOOST_LOG_TRIVIAL(warning) << "Skipping empty entry " << dir_entry;
            continue;
        }

        if (fs::is_directory(dir_entry))
        {
            if (fs::is_symlink(dir_entry))
            {
                BOOST_LOG_TRIVIAL(warning) << "Symlinks not supported , skipping " << dir_entry;
                continue;
            }

            index = recurse_parse_directory(out_file, temp_file, dir_entry, index);
        }

        // file conversions!
        else {
            file_node node;
            node.start = ftell(temp_file);
            node.type = FILE;

            std::FILE* input = fopen(entry_path.c_str(), "rb");
            node.size = copy_file_contents(temp_file, input);
            fclose(input);

            // copy failed
            if (!node.size)
                continue;

            write_data(out_file, &node);
        }

        entry.node_index = index++;

        entries.emplace_back(entry);
    }

    file_node dir_node;
    dir_node.start = ftell(temp_file);
    dir_node.size = sizeof(directory) + entries.size() * sizeof(directory_entry);
    dir_node.type = DIRECTORY;
    write_data(out_file, &dir_node);

    directory d_head;
    d_head.num_entries = entries.size();
    write_data(temp_file, &d_head);
    write_data(temp_file, entries.data(), entries.size());

    return index;
}

bool package_directory(const std::string &directory, const std::string &output, const std::string &name, const std::string &author, const uint32_t version)
{
    const mount_path tpk_info = mount_path(name, directory);

    const auto path_abs = fs::absolute(directory);
    const auto out_abs  = fs::absolute(output);

    if (!fs::is_directory(path_abs)) {
        BOOST_LOG_TRIVIAL(error) << "Failed to package '" << tpk_info << "', could not resolve as a directory";
        return false;
    }

    else if (fs::is_empty(path_abs)) {
        BOOST_LOG_TRIVIAL(error) << "Failed to package '" << tpk_info << "', empty directory";
        return false;
    }

    header head;
    head.dat_version = version;

    // name
    if (name.empty()) {
        BOOST_LOG_TRIVIAL(error) << "Could not write TPK file '" << tpk_info
            << "', name cannot be null";
        return false;
    }

    else if (name.length() > header::MAX_NAME_LEN)
        BOOST_LOG_TRIVIAL(warning) << "TPK name '" << name << "', length(" << name.length() << ')'
            << " exceeds max length of " << header::MAX_NAME_LEN
            << ", trucating to: " << name.substr(0, header::MAX_NAME_LEN);

    memcpy(head.name, name.data(), std::min(header::MAX_NAME_LEN, name.length()));

    // author
    if (author.empty())
        BOOST_LOG_TRIVIAL(warning) << "No author name specified for writing TPK '" << tpk_info << '\'';

    else if (author.length() > header::MAX_AUTHOR_LEN)
        BOOST_LOG_TRIVIAL(warning) << "TPK author '" << author << "', length(" << author.length() << ')'
            << "exceeds max length of " << header::MAX_AUTHOR_LEN
            << ", trucating to: " << author.substr(0, header::MAX_AUTHOR_LEN);

    memcpy(head.author, author.data(), std::min(header::MAX_AUTHOR_LEN, author.length()));

    std::FILE* out_file = fopen(out_abs.c_str(), "wb");
    if (!out_file)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to write to file '" << output << '\'';
        return false;
    }

    write_data(out_file, &head);

    const fs::path temp_path = fs::temp_directory_path().append(out_abs.filename().string() + ".dat");
    std::FILE* temp_dat = fopen(temp_path.c_str(), "wb");
    if (!temp_dat)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to create temp file: " << temp_path;
        fclose(out_file);
        return false;
    }

    BOOST_LOG_TRIVIAL(trace) << "Created temp file: " << temp_path;

    head.num_nodes = recurse_parse_directory(out_file, temp_dat, path_abs) + 1;
    fclose(temp_dat);

    temp_dat = fopen(temp_path.c_str(), "rb");
    copy_file_contents(out_file, temp_dat);

    fclose(temp_dat);

    // go back and write node count
    fflush(out_file);
    fseek(out_file, 0L + offsetof(header, num_nodes), SEEK_SET);
    write_data(out_file, &head.num_nodes);

    fclose(out_file);

    fs::remove(temp_path);

    BOOST_LOG_TRIVIAL(trace) << "Succesfully created TPK file: " << output;

    return true;
}

} /* end of namespace tavern::file::tpk */
