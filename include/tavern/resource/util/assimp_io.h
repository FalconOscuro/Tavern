#ifndef ASSIMP_IO_H
#define ASSIMP_IO_H

#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>

#include "assimp/Importer.hpp"
#include "tavern/file/imount.h"

namespace tavern::resource::util {

class assimp_iosystem final: public Assimp::IOSystem
{
public:
    assimp_iosystem() = default;
    ~assimp_iosystem() = default;

    bool Exists(const char* file) const override;
    char getOsSeparator() const override {
        return '/';
    }

    Assimp::IOStream* Open(const char* file, const char* mode) override;
    void Close(Assimp::IOStream* file) override;

    bool CreateDirectory(const std::string& path) override;
    bool DeleteFile(const std::string& path) override;
}; /* end of class assimp_iosystem final: public Assimp::IOSystem */

class assimp_iostream final: public Assimp::IOStream
{
    friend assimp_iosystem;

protected:
    assimp_iostream(file::ifile* file);

public:
    ~assimp_iostream() = default;

    size_t Read(void* buf, size_t size, size_t count) override;
    size_t Write(const void* buf, size_t size, size_t count) override;

    aiReturn Seek(size_t offset, aiOrigin origin) override;
    size_t Tell() const override;
    size_t FileSize() const override;

    void Flush() override;

private:

    file::file_handle m_file;
}; /* end of class assimp_iostream final: public Assimp::IOStream */

} /* end of namespace tavern::resource::util */

#endif /* end of define guard ASSIMP_IO_H */
