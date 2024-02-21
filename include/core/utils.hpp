#pragma once

#include <array>

namespace Airship::Utils
{
    template <typename value_type, size_t D>
    class Point
    {
    public:
        Point() = default;

        // Source: https://stackoverflow.com/questions/8158261/templates-how-to-control-number-of-constructor-args-using-template-variable
        template <typename... Args>
        Point(Args... args) : coords{value_type(args)...} {
            static_assert(sizeof...(Args) == D, "Wrong number of arguments");
        }

        value_type& operator[](size_t idx) {
            assert(idx < D);
            return coords[idx];
        }

        value_type operator[](size_t idx) const {
            static_assert(idx < D);
            return coords[idx];
        }

        value_type& x()
        {
            return coords[0];
        }

        template<int dims = D, typename = std::enable_if_t<dims >= 2>>
        value_type& y()
        {
            return coords[1];
        }

        template<int dims = D, typename = std::enable_if_t<dims >= 3>>
        value_type& z()
        {
            return coords[2];
        }

    private:
        std::array<value_type, D> coords;
    };
}
