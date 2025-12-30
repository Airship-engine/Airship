#pragma once

#include <array>
#include <cassert>
#include <cstddef>

namespace Airship::Utils
{
    template <typename value_type, std::size_t D>
    class Point
    {
    public:
        Point() = default;

        // Source: https://stackoverflow.com/questions/8158261/templates-how-to-control-number-of-constructor-args-using-template-variable
        template <typename... Args>
        Point(Args... args) : m_Coords{value_type(args)...} {
            static_assert(sizeof...(Args) == D, "Wrong number of arguments");
        }

        value_type& operator[](std::size_t idx) {
            assert(idx < D);
            return m_Coords[idx];
        }

        value_type operator[](std::size_t idx) const {
            static_assert(idx < D);
            return m_Coords[idx];
        }

        value_type& x()
        {
            return m_Coords[0];
        }

        template<int dims = D>
        value_type& y()
        requires (dims >= 2) {
            return m_Coords[1];
        }

        template<int dims = D>
        value_type& z()
        requires (dims >= 3) {
            return m_Coords[2];
        }

    private:
        std::array<value_type, D> m_Coords;
    };
}
