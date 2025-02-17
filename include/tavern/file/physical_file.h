#ifndef PHYSICAL_FILE_H
#define PHYSICAL_FILE_H

#include "ifile.h"

#include <cstdio>

namespace tavern {

class physical_file : public file
{
public:

    explicit physical_file(const std::string& path):
        file(path)
    {}

    ~physical_file();

    bool open() override;
    void close() override;

    bool is_open() const override;

    char get_char() override;
    size_t get_str(char* s, const size_t len) override;

private:

    std::FILE* m_file = NULL;
};

} /* namespace tavern */

#endif /* end of include guard: PHYSICAL_FILE_H */
