module;
#include "VT_Export"

export module VT.GraphicsContext;

namespace VT
{
class VT_ENGINE_EXPORT GraphicsContext
{
public:
    virtual void Init()       = 0;
    virtual void SwapBuffer() = 0;
};
} // namespace VT
