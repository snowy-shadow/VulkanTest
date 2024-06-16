module;
#include <vector>
#include "VT_Export"
export module LayerStack;
import Layer;

export namespace VT
{

/* Render order >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 * [Overlay1, Overlay2, Overlay3, ..., Layer1, Layer2, Layer3, ...]
 *                                        ^
 *                                 m_LayerBeginIndex
 *
 */
export class LayerStack
{
public:
    VT_ENGINE_EXPORT void PushOverlay(Layer*);
    VT_ENGINE_EXPORT Layer* RemoveOverlay(Layer*);

    VT_ENGINE_EXPORT void PushLayer(Layer*);
    VT_ENGINE_EXPORT Layer* RemoveLayer(Layer*);

    VT_ENGINE_EXPORT LayerStack() = default;
    VT_ENGINE_EXPORT ~LayerStack();

    VT_ENGINE_EXPORT std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
    VT_ENGINE_EXPORT std::vector<Layer*>::iterator end() { return m_Layers.end(); }

    VT_ENGINE_EXPORT std::vector<Layer*>::const_iterator cbegin() const { return m_Layers.cbegin(); }
    VT_ENGINE_EXPORT std::vector<Layer*>::const_iterator cend() const { return m_Layers.cend(); }

private:
    std::vector<Layer*> m_Layers;
    unsigned int m_LayerBeginIndex {0};
};
} // namespace VT
