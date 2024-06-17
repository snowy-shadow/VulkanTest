module;
#include "VT_Export"
export module VT.ImGuiLayer;
import VT.Layer;

export namespace VT
{
class VT_ENGINE_EXPORT ImGuiLayer : public Layer
{
public:
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate() override;
    void OnEvent(Event& E) override;
};
} // namespace VT
