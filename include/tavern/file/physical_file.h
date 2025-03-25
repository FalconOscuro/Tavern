#ifndef PHYSICAL_FILE_H
#define PHYSICAL_FILE_H

#include "ifile.h"

#include <cstdio>

namespace tavern::file {

class physical_file : public ifile
{
public:

    explicit physical_file(const std::string& path):
        ifile(path)
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
    void seek_start() override;
    size_t pos() const override;

    size_t size() const override;

private:

    std::FILE* m_file = NULL;
};

} /* namespace tavern::file */

#endif /* end of include guard: PHYSICAL_FILE_H */
