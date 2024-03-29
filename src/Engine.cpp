#include "Engine.hpp"

#include "graphics/nuklear.hpp"
#include "graphics/Camera.hpp"
#include "EngineState.hpp"

Engine::Engine() = default;

int Engine::run(int argc, char* argv[]) {

    onCreate();
    onUpdate();

    return 0;
}

void Engine::onCreate() {
    mainCreate();
}

void Engine::onUpdate() {
    bool anyone_window_visible = true;
    auto timer = Timer();

    while (anyone_window_visible) {
        engine_state().updateDeltaTime();
        engine_state()._fps_update();

        auto wnds = std::vector(_windows.begin(), _windows.end());

        anyone_window_visible = false;

        for (auto& wnd : wnds)
            anyone_window_visible |= wnd.first->is_visible();


        if (anyone_window_visible) {
            // Event update
            for (auto& wnd : wnds)
                if (wnd.first->is_visible())
                    wnd.first->eventUpdate();

            // Do other stuff
            if (physic_simulation) {
                physic_simulation->update();
            }

            for (auto& wnd : wnds)
                if (wnd.first->is_visible())
                    wnd.first->update(timer.tick().secf());

            // Renderer
            for (auto& wnd : wnds)
                if (wnd.first->is_visible())
                    wnd.first->render();
        }


        // Remove closed windows
        for (auto& wnd : wnds) {
            if (!wnd.first->is_visible() && wnd.second.destroy_on_close) {
                wnd.first->close();
                removeWindow(wnd.first);
            }
        }
    }
}

void Engine::addWindow(const std::shared_ptr<Window>& window, const WindowParams& params) {
    _windows.emplace(window, params);
}

void Engine::removeWindow(const std::shared_ptr<Window>& window) {
    _windows.erase(window);
}
