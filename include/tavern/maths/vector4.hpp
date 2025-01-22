#ifndef VECTOR4_HPP
#define VECTOR4_HPP

namespace tavern::maths {

template <typename T>
struct vector4 {

    vector4():
        x(), y(), z(), w()
    {}

    vector4(T x, T y, T z, T w):
        x(x), y(y), z(z), w(w)
    {}

    template <typename T2>
    vector4(const vector4<T2>& v):
        x(static_cast<T>(v.x)), y(static_cast<T>(v.y)), z(static_cast<T>(v.z)), w(static_cast<T>(v.w))
    {}

    T x;
    T y;
    T z;
    T w;
}; /* end of struct vector4<T> */

typedef vector4<int>    vector4i;
typedef vector4<float>  vector4f;
typedef vector4<double> vector4d;

} /* end of namespace tavern::maths */

#endif /* end of define guard VECTOR4_HPP */
