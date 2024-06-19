module;
#include "VT_Export"

#include <utility>

export module VT.Input;

import VT.Window;

export namespace VT
{
class VT_ENGINE_EXPORT Input
{
public:
    virtual bool IsKeyPressed(int KeyCode)        = 0;
    virtual bool IsMouseButtonPressed(int Button) = 0;
    virtual std::pair<float, float> GetMouseXY()  = 0;

    static Input& Create(const Window& Window);

    virtual ~Input() = default;

protected:
    Input() = default;
};
} // namespace VT
