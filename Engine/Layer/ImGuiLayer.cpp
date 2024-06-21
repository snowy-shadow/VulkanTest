module VT.ImGuiLayer;
import VT.Event;
import VT.Timestep;

namespace VT
{
void ImGuiLayer::OnAttach() {}
void ImGuiLayer::OnDetach() {}
void ImGuiLayer::OnUpdate(const Timestep& Timestep) {}
void ImGuiLayer::OnEvent(Event& E) {}

ImGuiLayer::ImGuiLayer() {}
ImGuiLayer::~ImGuiLayer() {}
} // namespace VT
