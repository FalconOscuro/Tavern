#ifndef PHYSICAL_FILE_H
#define PHYSICAL_FILE_H

#include "ifile.h"

#include <cstdio>

namespace tavern::file {

class physical_file : public ifile
{
public:

    explicit physical_file(const mount_path& file_info, const std::string_view mount_path):
        ifile(file_info), m_absolute_path(std::string(mount_path) + std::string(file_info.get_path()))
    {}

    explicit physical_file(const std::string_view absolute_path):
        ifile(mount_path("unmounted", absolute_path)), m_absolute_path(absolute_path)
    {}

    ~physical_file();

    bool open() override;
    void close() override;

    bool is_open() const override;
    bool eof() const override;

    [[nodiscard]] char peek_char() const override;
    char get_char() override;
    [[nodiscard]] size_t get_str(char* s, const size_t len) override;

    long seek(long offset) override;
    void seek_start(const size_t offset = 0) override;
    size_t pos() const override;

    size_t size() const override;

private:

    std::FILE* m_file = NULL;

    const std::string m_absolute_path;
};

} /* namespace tavern::file */

#endif /* end of include guard: PHYSICAL_FILE_H */
