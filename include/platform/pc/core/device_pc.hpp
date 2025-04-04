#pragma once

struct GLFWwindow;
struct GLFWmonitor;

namespace bee
{

class Device
{
public:
    bool ShouldClose();
    GLFWwindow* GetWindow();
    void CloseWindow();
    GLFWmonitor* GetMonitor();
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    Device();
    ~Device();

private:
    friend class EngineClass;
    void Update();

    GLFWwindow* m_window = nullptr;
    GLFWmonitor* m_monitor = nullptr;
    bool m_vsync = false;
    bool m_fullscreen = false;
    int m_width = -1;
    int m_height = -1;
};

} // namespace bee
