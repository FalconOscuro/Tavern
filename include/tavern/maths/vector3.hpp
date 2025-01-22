#ifndef VECTOR3_HPP
#define VECTOR3_HPP

namespace tavern::maths {

template <typename T>
struct vector3 {

    vector3():
        x(), y(), z()
    {}

    vector3(T x, T y, T z):
        x(x), y(y), z(z)
    {}

    template <typename T2>
    vector3(const T2* v):
        x(static_cast<T>(v[0])), y(v[1]), z(v[2])
    {}

    template <typename T2>
    vector3(const vector3<T2>& v):
        x(static_cast<T>(v.x)), y(static_cast<T>(v.y)), z(static_cast<T>(v.z))
    {}

    template <typename T2>
    vector3& operator=(const T2* v) {
        x = static_cast<T>(v[0]);
        y = static_cast<T>(v[1]);
        z = static_cast<T>(v[2]);
        return *this;
    }

    template <typename T2>
    vector3& operator=(const vector3<T2>& v) {
        x = static_cast<T>(v.x);
        y = static_cast<T>(v.y);
        z = static_cast<T>(v.z);
        return *this;
    }

    T x;
    T y;
    T z;
}; /* end of struct vector3<T> */

typedef vector3<int>    vector3i;
typedef vector3<float>  vector3f;
typedef vector3<double> vector3d;

} /* end of namespace tavern::maths */

#endif /* end of define guard VECTOR3_HPP */
