#include "tavern/core/cantrip.h"

#include <ryml.hpp>
#include <ryml_std.hpp>

#include <cantrip/scanner/scanner.hpp>
#include <cantrip/parser/parser.h>
#include <cantrip/analyzer/semantic.h>

#include "tavern/core/file_system.h"

namespace cantrip {

bool read(const ryml::ConstNodeRef& n, version* val)
{
    n["major"] >> val->major;
    n["minor"] >> val->minor;

    if (n.has_child("patch"))
        n["patch"] >> val->patch;

    else
        val->patch = 0;

    return true;
}

bool read(const ryml::ConstNodeRef& n, module_info* val)
{
    n["author"] >> val->author;
    n["email"] >> val->email;

    n["name"] >> val->name;
    n["description"] >> val->description;

    n["module_version"] >> val->module_version;
    n["cantrip_version"] >> val->cantrip_version;

    return true;
}

}

namespace tavern::core {

void recurse_get_cantrip_files(const file::idir* dir, std::vector<file::file_handle>& src_files)
{
    if (dir == nullptr)
        return;

    // find all .can files
    for (size_t i = 0; i < dir->num_files(); ++i)
    {
        if (dir->get_filename(i).get_extension() != "can")
            continue;

        file::file_handle src_file = dir->get_file(i);
        if (src_file != nullptr)
            src_files.emplace_back(src_file);
    }

    // recurse subdirs, circular tpk trees break this
    for (size_t i = 0; i < dir->num_dirs(); ++i)
        recurse_get_cantrip_files(dir->get_dir(i).get(), src_files);
}

bool cantrip_modules::load_module(const file::mount_path& module_path)
{
    file::dir_handle module_root_dir = file_system::singleton().load_dir(module_path);

    file::file_handle module_config_file = file_system::singleton().load_file(file::mount_path(module_path.get_identifier(), std::string(module_path.get_path()) + '/' + "cantrip.yml"));

    // failed to load dir
    if (module_root_dir == nullptr || module_config_file == nullptr)
        return false;

    else if (!module_config_file->open())
        return false;

    const size_t file_size = module_config_file->size();

    std::unique_ptr<char[]> config_contents = std::make_unique<char[]>(file_size + 1);

    // fail to read file
    if (!module_config_file->get_str(config_contents.get(), file_size))
        return false;

    config_contents[file_size] = '\0';

    cantrip::module module;
    // make try block
    {
        // ryml parse
        const ryml::Tree tree = ryml::parse_in_place(config_contents.get());
        const ryml::ConstNodeRef root = tree.rootref();

        root >> module.info;
    }

    // CHECK IF ALREADY LOADED

    std::vector<file::file_handle> module_src_files;
    recurse_get_cantrip_files(module_root_dir.get(), module_src_files);

    // scan files
    std::vector<cantrip::token> tokens;
    {
        cantrip::scanner scanner = cantrip::scanner(module_src_files);

        while (scanner.is_open())
        {
            tokens.emplace_back(scanner.peek());

            if (scanner.peek() == cantrip::MODULE_END)
                scanner.close();
            scanner.pop();
        }
    }

    // ensure last token is MODULE_END
    
    // parse
    {
        cantrip::parser parser = cantrip::parser(tokens);
        tokens.clear();

        parser.parse_module(module);
    }

    // semantic analysis
    {
        cantrip::analyzer::semantic semantic_analyzer;

        semantic_analyzer.analyze_module(&module);
    }

    return true;
}

} /* namespace tavern::core */
