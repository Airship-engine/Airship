#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

#include "addons.h"
#include "color.h"
#include "core/utils.hpp"
#include "grid.h"
#include "opengl/renderer.h"

constexpr Airship::Color SNAKE_COLOR = Airship::Colors::White;

using ivec2 = Airship::Utils::Point<int, 2>;
using vec2 = Airship::Utils::Point<float, 2>;

template <typename T>
class RingBuffer {
public:
    RingBuffer(size_t capacity) : head(capacity - 1), m_Capacity(capacity) { m_Data.resize(capacity); }
    T& PushHead() {
        assert(count < m_Capacity);
        count++;
        head = (head - 1) % m_Capacity;
        return GetElem(0);
    };
    T& PushTail() {
        assert(count < m_Capacity);
        count++;
        return GetElem(count - 1);
    };
    void PopTail() {
        assert(count > 0);
        count--;
    };
    void SetElem(size_t log_idx, T&& elem) {
        size_t phys_idx = GetIdx(log_idx);
        m_Data[phys_idx] = std::move(elem);
    }
    // 0 = head, count = tail
    T& GetElem(size_t logical) { return m_Data[GetIdx(logical)]; }
    [[nodiscard]] const T& GetElem(size_t logical) const { return m_Data[GetIdx(logical)]; }
    [[nodiscard]] size_t GetCount() const { return count; }
    std::vector<T*> Linearize() {
        std::vector<T*> ret(count);
        for (size_t i = 0; i < count; i++) {
            ret[i] = &m_Data[GetIdx(i)];
        }
        return ret;
    }

private:
    [[nodiscard]] size_t GetIdx(size_t logical) const {
        assert(logical < count);
        return (head + logical) % m_Capacity;
    }
    size_t head;
    size_t count = 0;
    size_t m_Capacity;
    std::vector<T> m_Data;
};

// clang-format off
constexpr std::array<ivec2, 6> canonicalSquare =
    {ivec2{0, 0}, ivec2{0, 1}, ivec2{1, 1},
     ivec2{0, 0}, ivec2{1, 1}, ivec2{1, 0}};
// clang-format on

inline std::array<ivec2, 6> CreateSquare(ivec2 pos) {
    std::array<ivec2, 6> ret{};
    for (size_t i = 0; i < canonicalSquare.size(); i++) {
        ret.at(i) = pos + canonicalSquare.at(i);
    }
    return ret;
}

class SnakeCell {
public:
    SnakeCell() = default; // uninitialized Streams
    void Initialize(ivec2 pos, const Grid<2>* grid) {
        m_Grid = grid;
        m_GridPos = pos;

        m_MeshData.addStream<vec2>("Position", Airship::ShaderDataType::Float2);
        m_MeshData.addStream<Airship::Color>("Color", Airship::ShaderDataType::Float4);

        std::array<ivec2, canonicalSquare.size()> square = CreateSquare(m_GridPos); // on grid
        std::vector<vec2> positions(square.size()); // on screen
        std::vector<Airship::Color> colors(square.size());
        for (size_t i = 0; i < square.size(); i++) {
            positions[i] = m_Grid->pos(square.at(i));
            colors[i] = SNAKE_COLOR;
        }
        OwningStream<vec2>& positionStream = m_MeshData.getStream<vec2>("Position");
        positionStream.data() = positions; // on screen
        positionStream.invalidate();

        OwningStream<Airship::Color>& colorStream = m_MeshData.getStream<Airship::Color>("Color");
        colorStream.data() = colors;
        colorStream.invalidate();
        m_MeshData.setVertexCount(canonicalSquare.size());
        initialized = true;
    }

    void draw(const Airship::Renderer& renderer, const Airship::Material& mat) {
        assert(initialized);
        m_MeshData.draw(renderer, mat);
    }
    ivec2 pos() const { return m_GridPos; }

private:
    bool initialized = false;
    ivec2 m_GridPos = ivec2(0, 0);
    const Grid<2>* m_Grid = nullptr;
    DynamicMesh m_MeshData;
};

class Apple {
public:
    Apple(ivec2 pos, const Grid<2>* grid, const Airship::Color& color) : m_GridPos(pos), m_Grid(grid) {
        m_MeshData.addStream<vec2>("Position", Airship::ShaderDataType::Float2);
        SetPos(pos);

        std::array<ivec2, canonicalSquare.size()> square = CreateSquare(m_GridPos); // on grid
        std::vector<Airship::Color> colors(square.size());
        for (size_t i = 0; i < square.size(); i++) {
            colors[i] = color;
        }

        OwningStream<Airship::Color>& colorStream =
            m_MeshData.addStream<Airship::Color>("Color", Airship::ShaderDataType::Float4);
        colorStream.data() = colors;
        colorStream.invalidate();
        m_MeshData.setVertexCount(canonicalSquare.size());
    }

    void draw(const Airship::Renderer& renderer, Airship::Pipeline* pipeline) { m_MeshData.draw(renderer, pipeline); }
    void draw(const Airship::Renderer& renderer, const Airship::Material& mat) { m_MeshData.draw(renderer, mat); }
    ivec2 pos() const { return m_GridPos; }
    void SetPos(ivec2 gridPos) {
        m_GridPos = gridPos;
        std::array<ivec2, canonicalSquare.size()> square = CreateSquare(m_GridPos); // on grid
        std::vector<vec2> positions(square.size()); // on screen
        std::vector<Airship::Color> colors(square.size());
        for (size_t i = 0; i < square.size(); i++) {
            positions[i] = m_Grid->pos(square.at(i));
        }
        OwningStream<vec2>& positionStream = m_MeshData.getStream<vec2>("Position");
        positionStream.data() = positions; // on screen
        positionStream.invalidate();
    }

private:
    ivec2 m_GridPos = ivec2(0, 0);
    const Grid<2>* m_Grid = nullptr;
    DynamicMesh m_MeshData;
};

enum class Direction : uint8_t {
    Left,
    Up,
    Right,
    Down
};

inline ivec2 toVec(Direction dir) {
    switch (dir) {
    case Direction::Left:
        return ivec2{-1, 0};
    case Direction::Up:
        return ivec2{0, 1};
    case Direction::Right:
        return ivec2{1, 0};
    case Direction::Down:
        return ivec2{0, -1};
    }
}

inline ivec2 operator+(const ivec2& lhs, const ivec2& rhs) {
    return {lhs.x() + rhs.x(), lhs.y() + rhs.y()};
}
inline vec2 operator*(const ivec2& lhs, float scale) {
    return {static_cast<float>(lhs.x()) * scale, static_cast<float>(lhs.y()) * scale};
}

class Snake {
public:
    Snake(const Grid<2>* grid) : m_Grid(grid), m_Cells(256) {}
    void Initialize(ivec<2> location) {
        assert(m_Cells.GetCount() == 0);
        auto& newHead = m_Cells.PushHead();
        newHead.Initialize(location, m_Grid);
    }
    void Grow() {
        const auto& tailCell = m_Cells.GetElem(m_Cells.GetCount() - 1);
        auto& newTail = m_Cells.PushTail();
        newTail.Initialize(tailCell.pos(), m_Grid);
    }
    [[nodiscard]] ivec2 HeadPos() const { return m_Cells.GetElem(0).pos(); }
    void SetDir(Direction dir) { m_MoveDir = dir; }
    [[nodiscard]] Direction GetDir() { return m_MoveDir; }
    void PopTail() { m_Cells.PopTail(); }
    void draw(const Airship::Renderer& renderer, const Airship::Material& mat) {
        for (int i = static_cast<int>(m_Cells.GetCount()) - 1; i >= 0; --i) {
            auto& cell = m_Cells.GetElem(i);
            cell.draw(renderer, mat);
        }
    }

    [[nodiscard]] bool IntersectsSelf(ivec2 pos) const {
        for (size_t i = 0; i < m_Cells.GetCount(); i++) {
            if (pos == m_Cells.GetElem(i).pos()) {
                return true;
            }
        }
        return false;
    }

    void Update(ivec2 applePos) {
        PROFILE_FUNCTION();
        const auto& headPos = HeadPos();
        const auto& nextPos = m_Grid->getCoord(headPos + toVec(m_MoveDir));
        if (IntersectsSelf(nextPos)) {
            m_IsAlive = false;
            return;
        }
        if (nextPos != applePos) {
            PopTail();
        }
        auto& newHead = m_Cells.PushHead();
        newHead.Initialize(nextPos, m_Grid);
    }

    [[nodiscard]] bool IsAlive() const { return m_IsAlive; }

private:
    const Grid<2>* m_Grid;
    Direction m_MoveDir = Direction::Right;
    RingBuffer<SnakeCell> m_Cells;
    bool m_IsAlive = true;
};