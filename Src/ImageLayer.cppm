module;
#include "EngineMacro.h"
export module ImageLayer;

import VulkanTest;

export class ImageLayer final : public VT::Layer
{
public:
    void OnAttach() override {}
    void OnDetach() override {}
    void OnUpdate() override { VT_INFO("Layer update"); }
    void OnEvent(VT::Event& E) override { VT_INFO("Layer Event : {}", E); }
};
