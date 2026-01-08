#include <memory>
#include <vector>

#include "color.h"
#include "core/application.h"
#include "opengl/renderer.h"
#include "utils.hpp"

using vec3 = Airship::Utils::Point<float, 3>;

struct TriangleVertexData {
    vec3 position;
    Airship::Color color;
};

class Game : public Airship::Application {
public:
    Game() : Airship::Application(600, 800, "Falling Triangles Example") {}

    void OnStart() override;
    void OnGameLoop(float elapsed) override;

private:
    void CreatePipelines();
    // TODO: Change to platform management abstraction. Renderer classes have
    // inert constructors plus a factory method from the platform to create/validate.
    // Then we can remove these unique pointers.
    std::unique_ptr<Airship::Pipeline> m_TriPipeline, m_BGPipeline;
    std::unique_ptr<Airship::Buffer> m_TriBuffer, m_BGBuffer, m_BGHuesBuffer;
    Airship::Mesh m_TriMesh, m_BGMesh;

    std::vector<TriangleVertexData> m_TriVerts;
    std::vector<vec3> m_BGPositions;
    std::vector<float> m_BGHues;
    float m_BGHue = 0.0f;

    float m_TimeSinceSpawn = 1.0f;
    int m_LowestTriangleIndex = 0;
};
