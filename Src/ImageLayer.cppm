module;
#include "EngineMacro.h"
// compiler bug
#include <format>
export module ImageLayer;

import VulkanTest;

export class ImageLayer final : public VT::Layer
{
public:
    void OnAttack() override {}
    void OnDetach() override {}
    void OnUpdate() override { VT_INFO("Layer update"); }
    void OnEvent(VT::Event& E) override { VT_INFO("Layer Event : {}", E); }
};
