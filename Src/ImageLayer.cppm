module;
#include "EngineMacro.h"
export module ImageLayer;

import VulkanTest;

export class ImageLayer final : public VT::Layer
{
public:
    void OnAttach() override {}
    void OnDetach() override {}
    void OnUpdate(const VT::Timestep&) override { VT_INFO("Layer update"); }
    void OnEvent(VT::Event& E) override { (void)E; } // VT_INFO("Layer Event : {}", E); }
};
