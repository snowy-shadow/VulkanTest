module;
#include "VT_Export"

#include <utility>

export module VT.Input;

import VT.Window;
import VT.Util;
import VT.Key;

export namespace VT
{
class VT_ENGINE_EXPORT Input
{
public:
    virtual bool IsKeyPressed(Key KeyCode) const        = 0;
    virtual bool IsMouseButtonPressed(Key Button) const = 0;
    virtual std::pair<float, float> GetMouseXY() const  = 0;

    static Input* Create(const Window& Window);

    virtual ~Input() = default;

protected:
    Input() = default;
};
} // namespace VT
