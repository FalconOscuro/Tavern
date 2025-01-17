#ifndef VECTOR3_HPP
#define VECTOR3_HPP

namespace tavern::maths {

template <typename T>
struct vector3 {

    vector3():
        X(), Y(), Z()
    {}

    vector3(T X, T Y, T Z):
        X(X), Y(Y), Z(Z)
    {}

    template <typename T2>
    vector3(const vector3<T2>& v):
        X(static_cast<T>(v.X)), Y(static_cast<T>(v.Y)), Z(static_cast<T>(v.Z))
    {}

    T X;
    T Y;
    T Z;
}; /* end of struct vector3<T> */

typedef vector3<int>    vector3i;
typedef vector3<float>  vector3f;
typedef vector3<double> vector3d;

} /* end of namespace tavern::maths */

#endif /* end of define guard VECTOR3_HPP */
