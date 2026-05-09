#pragma once

#include <array>
#include <cassert>
#include <cstddef>

namespace Airship::Utils {

// Based on boost::hash_combine
inline size_t hash_combine(size_t seed, size_t value) {
    return seed ^ (value + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

template <typename value_type, std::size_t D>
class Point {
public:
    Point() = default;

    // Source:
    // https://stackoverflow.com/questions/8158261/templates-how-to-control-number-of-constructor-args-using-template-variable
    template <typename... Args>
    constexpr Point(Args... args) : m_Coords{value_type(args)...} {
        static_assert(sizeof...(Args) == D, "Wrong number of arguments");
    }

    value_type& operator[](std::size_t idx) {
        assert(idx < D);
        return m_Coords[idx];
    }

    value_type operator[](std::size_t idx) const {
        assert(idx < D);
        return m_Coords[idx];
    }

    value_type& x() { return m_Coords[0]; }
    [[nodiscard]] const value_type& x() const { return m_Coords[0]; }

    template <int dims = D>
    value_type& y()
        requires(dims >= 2)
    {
        return m_Coords[1];
    }

    template <int dims = D>
    [[nodiscard]] const value_type& y() const
        requires(dims >= 2)
    {
        return m_Coords[1];
    }

    template <int dims = D>
    value_type& z()
        requires(dims >= 3)
    {
        return m_Coords[2];
    }

    template <int dims = D>
    [[nodiscard]] const value_type& z() const
        requires(dims >= 3)
    {
        return m_Coords[2];
    }

    bool operator==(const Point<value_type, D>& other) const {
        bool same = true;
        for (size_t i = 0; i < D; i++) {
            if (m_Coords[i] != other[i]) same = false;
        }
        return same;
    }

private:
    std::array<value_type, D> m_Coords;
};

template <typename T, size_t N>
inline Point<T, N> operator+(const Point<T, N>& lhs, const Point<T, N>& rhs) {
    Airship::Utils::Point<T, N> ret;
    for (size_t i = 0; i < N; i++) {
        ret[i] = lhs[i] + rhs[i];
    }
    return ret;
}

template <typename T, size_t N>
inline Point<T, N> operator-(const Point<T, N>& lhs, const Point<T, N>& rhs) {
    Airship::Utils::Point<T, N> ret;
    for (size_t i = 0; i < N; i++) {
        ret[i] = lhs[i] - rhs[i];
    }
    return ret;
}

template <typename T, size_t N>
inline Point<T, N> operator*(const Point<T, N>& lhs, float scale) {
    Airship::Utils::Point<T, N> ret;
    for (size_t i = 0; i < N; i++) {
        ret[i] = lhs[i] * scale;
    }
    return ret;
}

template <typename T, size_t N>
inline Point<T, N> operator/(const Point<T, N>& lhs, float scale) {
    Airship::Utils::Point<T, N> ret;
    for (size_t i = 0; i < N; i++) {
        ret[i] = lhs[i] / scale;
    }
    return ret;
}
} // namespace Airship::Utils
