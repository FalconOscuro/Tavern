#ifndef VIRTUAL_FILE_H
#define VIRTUAL_FILE_H

#include "ifile.h"

namespace tavern::file {

// Used for data stored internally within the program
class virtual_file final : public ifile
{
public:

    virtual_file(const char* const data, const std::string& path);
    ~virtual_file() = default;

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

    const char* const m_data;
    size_t m_pos = 0;

}; /* end of class virtual_file final : public ifile */

} /* end of namespace tavern::file */

#endif /* end of define guard VIRTUAL_FILE_H */
