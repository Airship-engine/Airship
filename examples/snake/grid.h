#pragma once

#include <cstddef>
#include <cstdlib>
#include <functional>

#include "utils.hpp"

template <size_t N>
using ivec = Airship::Utils::Point<int, N>;

template <size_t N>
using vec = Airship::Utils::Point<float, N>;

template <size_t N>
using onOOBFunc = std::function<ivec<N>(ivec<N>, ivec<N>)>;

// Maps from an integer space (on a lattice) to a floating-point space.
// When an OOB index is called, can set a callback to decide how to handle it.
// - Default: abort
template <size_t NDIMS>
class Grid {
public:
    Grid(float size, vec<NDIMS> offset, ivec<NDIMS> bounds) :
        m_CellSize(size), m_Offset(offset), m_Bounds(bounds), m_OOBCallback([](ivec<NDIMS> bounds, ivec<NDIMS> coords) {
            (void) bounds;
            (void) coords;
            std::abort();
            return coords;
        }) {}

    vec<NDIMS> pos(ivec<NDIMS> coords, bool boundsCheck = false) const {
        if (boundsCheck) coords = getCoord(coords);
        return remap(coords);
    }
    void SetOOBCallback(onOOBFunc<NDIMS> func) { m_OOBCallback = func; }
    [[nodiscard]] ivec<NDIMS> GetBounds() const { return m_Bounds; }

    // Get a position on the grid, handling OOB accesses as specified in the callback
    ivec<NDIMS> getCoord(ivec<NDIMS> coord) const {
        bool oob = false;
        for (size_t i = 0; i < NDIMS; i++) {
            if (coord[i] < 0 || coord[i] >= m_Bounds[i]) {
                oob = true;
                break;
            }
        }
        return oob ? m_OOBCallback(m_Bounds, coord) : coord;
    }

private:
    vec<NDIMS> remap(const ivec<NDIMS>& coord) const {
        vec<NDIMS> fcoord;
        for (size_t i = 0; i < NDIMS; i++) {
            fcoord[i] = coord[i];
        }
        return (fcoord * m_CellSize) + m_Offset;
    }
    float m_CellSize;
    vec<NDIMS> m_Offset;
    ivec<NDIMS> m_Bounds;
    onOOBFunc<NDIMS> m_OOBCallback;
};
