module;
#include "VT_Export"
export module VT.Layer;

import VT.Event;
import VT.Timestep;

namespace VT
{
export class VT_ENGINE_EXPORT Layer
{
public:
    virtual void OnAttach()                = 0;
    virtual void OnDetach()                = 0;
    virtual void OnUpdate(const Timestep&) = 0;
    virtual void OnEvent(Event& E)         = 0;

    virtual ~Layer() = default;
};

} // namespace VT
