#include "tavern/core/cantrip.h"

#include <boost/log/trivial.hpp>

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

namespace tavern {

cantrip_modules::~cantrip_modules() {
    shutdown();
}

bool cantrip_modules::init() {
    return true;
}

void cantrip_modules::shutdown() {
    unload_all_modules();
}

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
            src_files.emplace_back(std::move(src_file));
    }

    // recurse subdirs, circular tpk trees break this
    for (size_t i = 0; i < dir->num_dirs(); ++i)
        recurse_get_cantrip_files(dir->get_dir(i).get(), src_files);
}

std::shared_ptr<cantrip::module> cantrip_modules::load_module(const file::mount_path& module_path)
{
    file::dir_handle module_root_dir = file_system::singleton().load_dir(module_path);

    file::file_handle module_config_file = file_system::singleton().load_file(file::mount_path(module_path.get_identifier(), std::string(module_path.get_path()) + '/' + "cantrip.yml"));

    // failed to load dir
    if (module_root_dir == nullptr || module_config_file == nullptr)
        return nullptr;

    else if (!module_config_file->open())
        return nullptr;

    const size_t file_size = module_config_file->size();

    std::unique_ptr<char[]> config_contents = std::make_unique<char[]>(file_size + 1);

    // fail to read file
    if (!module_config_file->get_str(config_contents.get(), file_size))
        return nullptr;

    config_contents[file_size] = '\0';

    cantrip::module module;
    // make try block
    {
        // ryml parse
        const ryml::Tree tree = ryml::parse_in_place(config_contents.get());
        const ryml::ConstNodeRef root = tree.rootref();

        root >> module.info;
    }

    auto found = m_loaded_modules.find(module.info.name);

    if (found != m_loaded_modules.end())
    {
        if (module.info.module_version != found->second->info.module_version)
            BOOST_LOG_TRIVIAL(warning) << "Tried to load cantrip module '"
                << module.info.name << "' from '"
                << module_config_file->get_path() << "' but was already loaded, with version mismatch!";

        return found->second;
    }

    std::vector<file::file_handle> module_src_files;
    recurse_get_cantrip_files(module_root_dir.get(), module_src_files);

    // scan files
    std::vector<cantrip::token> tokens;
    {
        cantrip::scanner scanner = cantrip::scanner(std::move(module_src_files));

        while (scanner.is_open())
        {
            tokens.emplace_back(scanner.peek());

            if (scanner.peek() == cantrip::MODULE_END)
                scanner.close();
            scanner.pop();
        }
    }

    // ensure last token is MODULE_END
    if (tokens.back() != cantrip::MODULE_END)
    {
        BOOST_LOG_TRIVIAL(error) << "Failure whilst reading cantrip module '" << module.info.name << "', expected MODULE_END token!";
        return nullptr;
    }
    
    // parse
    try {
        cantrip::parser parser = cantrip::parser(std::move(tokens));
        parser.parse_module(module);
    }
    catch (const std::exception& e)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to parse cantrip module '"
            << module.info.name << "':\n"
            << e.what();
        return nullptr;
    }

    // semantic analysis
    try {
        cantrip::analyzer::semantic semantic_analyzer;

        semantic_analyzer.analyze_module(&module);
    }
    catch (const std::exception& e)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed semantic analysis of cantrip module '"
            << module.info.name << "':\n"
            << e.what();
        return nullptr;
    }

    return m_loaded_modules.emplace(module.info.name, std::make_shared<cantrip::module>(std::move(module))).first->second;
}

void cantrip_modules::unload_module(const std::string_view module_name)
{
    // UNIMPLEMENTED
    (void)module_name;

    // need to ensure no datatypes associated with module remains, should utilize module dependency info
}

std::shared_ptr<cantrip::module> cantrip_modules::get_module(const std::string_view module_name)
{
    auto found = m_loaded_modules.find(module_name);

    return found != m_loaded_modules.end() ? found->second : nullptr;
}

// UNIMPLEMENTED
void cantrip_modules::unload_all_modules()
{}

} /* namespace tavern::core */
