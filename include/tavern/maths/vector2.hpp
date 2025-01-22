#ifndef VECTOR2_HPP
#define VECTOR2_HPP

namespace tavern::maths {

template <typename T>
struct vector2 {

    vector2():
        x(), y()
    {}

    vector2(const T x, const T y):
        x(x), y(y)
    {}

    template <typename T2>
    vector2(const vector2<T2>& v):
      x(static_cast<T>(v.x)), y(static_cast<T>(v.y))
    {}

    T x;
    T y;
}; /* end of struct vector2<T> */

typedef vector2<int>    vector2i;
typedef vector2<float>  vector2f;
typedef vector2<double> vector2d;

} /* end of namespace tavern::maths */

#endif /* end of define guard VECTOR2_HPP */
