#ifndef VECTOR2_HPP
#define VECTOR2_HPP

namespace tavern::maths {

template <typename T>
struct vector2 {

    vector2():
        X(), Y()
    {}

    vector2(const T X, const T Y):
        X(X), Y(Y)
    {}

    template <typename T2>
    vector2(const vector2<T2>& v):
      X(static_cast<T>(v.X)), Y(static_cast<T>(v.Y))
    {}

    T X;
    T Y;
}; /* end of struct vector2<T> */

typedef vector2<int>    vector2i;
typedef vector2<float>  vector2f;
typedef vector2<double> vector2d;

} /* end of namespace tavern::maths */

#endif /* end of define guard VECTOR2_HPP */
