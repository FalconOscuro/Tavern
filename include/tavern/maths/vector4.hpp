#ifndef VECTOR4_HPP
#define VECTOR4_HPP

namespace tavern::maths {

template <typename T>
struct vector4 {

    vector4():
        X(), Y(), Z(), W()
    {}

    vector4(T X, T Y, T Z, T W):
        X(X), Y(Y), Z(Z), W(W)
    {}

    template <typename T2>
    vector4(const vector4<T2>& v):
        X(static_cast<T>(v.X)), Y(static_cast<T>(v.Y)), Z(static_cast<T>(v.Z)), W(static_cast<T>(v.W))
    {}

    T X;
    T Y;
    T Z;
    T W;
}; /* end of struct vector4<T> */

typedef vector4<int>    vector4i;
typedef vector4<float>  vector4f;
typedef vector4<double> vector4d;

} /* end of namespace tavern::maths */

#endif /* end of define guard VECTOR4_HPP */
