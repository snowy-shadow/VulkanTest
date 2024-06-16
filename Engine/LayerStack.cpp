module;
#include <vector>
#include <algorithm>
#include "EngineMacro.h"

// Compiler bug
#include <format>
module LayerStack;
import Layer;

#ifdef VT_ENABLE_MESSAGE
import Log;
#endif

namespace VT
{

LayerStack::~LayerStack()
{
    for (auto Layer : m_Layers)
    {
        delete Layer;
    }
}

void LayerStack::PushOverlay(Layer* pOverlay)
{
    VT_CORE_ASSERT(pOverlay != nullptr, "Overlay nullptr");
    m_Layers.emplace(m_Layers.begin() + m_LayerBeginIndex, pOverlay);
    m_LayerBeginIndex++;
}

Layer* LayerStack::RemoveOverlay(Layer* pOverlay)
{
    auto Iter = std::find(m_Layers.cbegin(), m_Layers.cend(), pOverlay);

    if (Iter == m_Layers.end())
    {
        return nullptr;
    }

    Layer* Item = *Iter;
    m_Layers.erase(Iter);
    m_LayerBeginIndex--;

    return Item;
}

void LayerStack::PushLayer(Layer* pLayer)
{
    VT_CORE_ASSERT(pLayer != nullptr, "Layer nullptr");
    m_Layers.emplace_back(pLayer);
}

Layer* LayerStack::RemoveLayer(Layer* pLayer)
{
    auto Iter = std::find(m_Layers.cbegin(), m_Layers.cend(), pLayer);

    if (Iter == m_Layers.end())
    {
        return nullptr;
    }

    Layer* Item = *Iter;
    m_Layers.erase(Iter);

    return Item;
}

} // namespace VT
