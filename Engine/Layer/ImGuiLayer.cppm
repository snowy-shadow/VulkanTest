module;
#include "VT_Export"
export module VT.ImGuiLayer;
import VT.Layer;
import VT.Timestep;

export namespace VT
{
class VT_ENGINE_EXPORT ImGuiLayer : public Layer
{
public:
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(const Timestep&) override;
    void OnEvent(Event& E) override;
};
} // namespace VT
