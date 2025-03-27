#ifndef TPK_PACKAGE_H
#define TPK_PACKAGE_H

#include <string>
#include <cstdint>

namespace tavern::file::tpk {

bool package_directory(const std::string& directory, const std::string& output, const std::string& name, const std::string& author, const uint32_t version = 0);

} /* end of namespace tavern::file::tpk */

#endif /* end of define guard TPK_PACKAGE_H */
