#include "cantrip/module/info.h"

namespace cantrip {

bool version::operator==(const version& rhs) const {
    return major == rhs.major && minor == rhs.minor && patch == rhs.patch;
}

bool version::operator!=(const version& rhs) const {
    return major != rhs.major || minor != rhs.minor || patch != rhs.patch;
}

bool version::operator<(const version& rhs) const {
    return major < rhs.major
        || (major == rhs.major && (minor < rhs.minor
            || (minor == rhs.minor && patch < rhs.patch)
        ))
    ;
}

bool version::operator>(const version& rhs) const {
    return major > rhs.major
        || (major == rhs.major && (minor > rhs.minor
            || (minor == rhs.minor && patch > rhs.patch)
        ))
    ;
}

bool version::operator<=(const version& rhs) const {
    return major < rhs.major
        || (major == rhs.major && (minor < rhs.minor
            || (minor == rhs.minor && patch <= rhs.patch)
        ))
    ;
}

bool version::operator>=(const version& rhs) const {
    return major > rhs.major
        || (major == rhs.major && (minor > rhs.minor
            || (minor == rhs.minor && patch >= rhs.patch)
        ))
    ;
}

} /* end of namespace cantrip */
