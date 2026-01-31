#pragma once

#include <cassert>
#include <vector>

#include "addons.h"
#include "core/utils.hpp"
#include "render/color.h"
#include "render/opengl/renderer.h"

constexpr float TALLY_WIDTH = 0.015;
constexpr float TALLY_GROUP_HMARGIN = 0.012;
constexpr float TALLY_GROUP_VMARGIN = 0.005;

class TallyGroup {
    using vec = Airship::Utils::Point<float, 2>;

public:
    constexpr static int MAX_TALLIES = 5;
    TallyGroup(vec offset, vec groupSize) : m_Offset(offset), m_GroupSize(groupSize) {
        m_MeshData.addStream<vec>("Position", Airship::ShaderDataType::Float2);
        m_MeshData.addStream<Airship::Color>("Color", Airship::ShaderDataType::Float4);
    }
    void increment() {
        assert(m_Count < MAX_TALLIES);
        auto& positionStream = m_MeshData.getStream<vec>("Position");
        std::vector<vec>& positions = positionStream.data();
        auto& colorStream = m_MeshData.getStream<Airship::Color>("Color");
        std::vector<Airship::Color>& colors = colorStream.data();
        if (m_Count == MAX_TALLIES - 1) {
            // Slashed tally
            vec shift = m_Offset;
            float width = TALLY_WIDTH * 1.4; // Assuming ~45 degree angle
            float startX = shift.x() + TALLY_GROUP_HMARGIN - (width / 2);
            float endX = shift.x() + m_GroupSize.x() - TALLY_GROUP_HMARGIN - (width / 2);
            positions.emplace_back(startX, shift.y());
            positions.emplace_back(startX + width, shift.y());
            positions.emplace_back(endX, m_GroupSize.y() + shift.y());
            positions.emplace_back(endX, m_GroupSize.y() + shift.y());
            positions.emplace_back(startX + width, shift.y());
            positions.emplace_back(endX + width, m_GroupSize.y() + shift.y());
            colors.emplace_back(Airship::Colors::White);
            colors.emplace_back(Airship::Colors::White);
            colors.emplace_back(Airship::Colors::White);
            colors.emplace_back(Airship::Colors::White);
            colors.emplace_back(Airship::Colors::White);
            colors.emplace_back(Airship::Colors::White);
        } else {
            // Vertical tally
            vec shift = m_Offset;
            float tally_offset = (m_GroupSize.x() - (2 * TALLY_GROUP_HMARGIN) - TALLY_WIDTH) / 3;
            shift.x() += TALLY_GROUP_HMARGIN + (float(m_Count) * tally_offset);
            float bottom = TALLY_GROUP_VMARGIN;
            float top = m_GroupSize.y() - TALLY_GROUP_VMARGIN;
            positions.emplace_back(shift + vec(0, bottom));
            positions.emplace_back(shift + vec(TALLY_WIDTH, bottom));
            positions.emplace_back(shift + vec(TALLY_WIDTH, top));
            positions.emplace_back(shift + vec(0, bottom));
            positions.emplace_back(shift + vec(TALLY_WIDTH, top));
            positions.emplace_back(shift + vec(0, top));
            colors.emplace_back(0.7f, 0.7f, 0.7f);
            colors.emplace_back(0.7f, 0.7f, 0.7f);
            colors.emplace_back(0.7f, 0.7f, 0.7f);
            colors.emplace_back(0.7f, 0.7f, 0.7f);
            colors.emplace_back(0.7f, 0.7f, 0.7f);
            colors.emplace_back(0.7f, 0.7f, 0.7f);
        }
        m_MeshData.setVertexCount(m_MeshData.vertexCount() + 6);
        positionStream.invalidate();
        colorStream.invalidate();
        m_Count++;
    }
    bool full() const { return m_Count == MAX_TALLIES; }
    void draw(Airship::Renderer& renderer, Airship::Pipeline& pipeline) { m_MeshData.draw(renderer, &pipeline); }

private:
    DynamicMesh m_MeshData;
    int m_Count = 0;
    vec m_Offset;
    vec m_GroupSize;
};

class Tallies {
    using vec = Airship::Utils::Point<float, 2>;

public:
    Tallies(vec offset, vec groupSize) : m_Offset(offset), m_TallyGroupSize(groupSize) {}
    void increment() {
        if (m_TallyGroups.empty() || m_TallyGroups.back().full()) {
            m_TallyGroups.emplace_back(m_Offset, m_TallyGroupSize);
            m_Offset.x() += m_TallyGroupSize.x();
        }
        m_TallyGroups.back().increment();
    }
    void draw(Airship::Renderer& renderer, Airship::Pipeline& pipeline) {
        for (auto& group : m_TallyGroups)
            group.draw(renderer, pipeline);
    }

private:
    vec m_Offset;
    vec m_TallyGroupSize;
    std::vector<TallyGroup> m_TallyGroups;
};
