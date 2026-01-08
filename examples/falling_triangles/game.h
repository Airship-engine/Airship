#include "core/application.h"
#include "render/opengl/renderer.h"

class Game : public Airship::Application {
public:
    Game() : Airship::Application(600, 800, "Falling Triangles Example") {}

    void OnStart() override;

protected:
    Airship::Renderer m_Renderer;
};
