module;
#include <cstdint>
#include <vector>
#include "VT_Export"
#include "EngineMacro.h"
export module VT.Buffer;

#ifdef VT_ENABLE_MESSAGE
import VT.Log;
#endif

export namespace VT
{

/* ====================================
 *          Shader Data Type
 * ====================================
 */
enum class ShaderDataType
{
    eNone,
    eFloat,
    eFloat2,
    eFloat3,
    eFloat4,
    eMat3,
    eMat4,
    eInt,
    eInt2,
    eInt3,
    eInt4,
    eBool
};

constexpr uint32_t ShaderDataTypeSize(ShaderDataType Type)
{
    switch (Type)
    {
        case ShaderDataType::eFloat:
            return sizeof(float);
        case ShaderDataType::eFloat2:
            return sizeof(float) * 2;
        case ShaderDataType::eFloat3:
            return sizeof(float) * 3;
        case ShaderDataType::eFloat4:
            return sizeof(float) * 4;

        case ShaderDataType::eMat3:
            return sizeof(float) * 3 * 3;
        case ShaderDataType::eMat4:
            return sizeof(float) * 4 * 4;

        case ShaderDataType::eInt:
            return sizeof(int);
        case ShaderDataType::eInt2:
            return sizeof(int) * 2;
        case ShaderDataType::eInt3:
            return sizeof(int) * 3;
        case ShaderDataType::eInt4:
            return sizeof(int) * 4;

        case ShaderDataType::eBool:
            return sizeof(bool);
    }

    VT_CORE_HALT("Unknown Shader data type");
#ifndef VT_ENABLE_ASSERT
    return 0;
#endif
}
constexpr uint32_t ComputeShaderDataElementCount(ShaderDataType Type)
{
    switch (Type)
    {
        case ShaderDataType::eFloat: return 1;
        case ShaderDataType::eFloat2: return 2;
        case ShaderDataType::eFloat3: return 3;
        case ShaderDataType::eFloat4: return 4;

        case ShaderDataType::eMat3: return 3 * 3;
        case ShaderDataType::eMat4: return 4 * 4;

        case ShaderDataType::eInt: return 1;
        case ShaderDataType::eInt2: return 2;
        case ShaderDataType::eInt3: return 3;
        case ShaderDataType::eInt4: return 4;

        case ShaderDataType::eBool: return 1;
    }

    VT_CORE_HALT("Unknown Shader data type");
#ifndef VT_ENABLE_ASSERT
    return 0;
#endif
}
/* ====================================
 *            BufferElement
 * ====================================
 */
struct VT_ENGINE_EXPORT BufferElement
{
    ShaderDataType Type;
    uint32_t Size;
    uint32_t Offset {0};
    bool Normalized;

    BufferElement(ShaderDataType Type, bool Normalized = false) :
        Type(Type), Size(ShaderDataTypeSize(Type)), Normalized(Normalized)
    {
    }

    constexpr uint32_t GetElementCount() const { return ComputeShaderDataElementCount(Type); }
};

/* ====================================
 *            BufferLayout
 * ====================================
 */
class BufferLayout
{
public:
    VT_ENGINE_EXPORT BufferLayout(const std::initializer_list<BufferElement>& Elements) : m_Elements(Elements)
    {
        ComputeOffsetAndStride();
    }

    VT_ENGINE_EXPORT void Init(const std::initializer_list<BufferElement>& Elements)
    {
        m_Elements = Elements;
        ComputeOffsetAndStride();
    }

    VT_ENGINE_EXPORT constexpr std::vector<BufferElement> GetElements() const { return m_Elements; }
    VT_ENGINE_EXPORT constexpr uint32_t GetStride() const { return m_Stride; }
    VT_ENGINE_EXPORT constexpr uint32_t GetSize() const { return m_Size; }

    VT_ENGINE_EXPORT std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
    VT_ENGINE_EXPORT std::vector<BufferElement>::iterator end() { return m_Elements.end(); }

    VT_ENGINE_EXPORT std::vector<BufferElement>::const_iterator cbegin() const { return m_Elements.cbegin(); }
    VT_ENGINE_EXPORT std::vector<BufferElement>::const_iterator cend() const { return m_Elements.cend(); }

public:
    BufferLayout() = default;
    BufferLayout(const BufferLayout&)                = default;
    BufferLayout& operator=(const BufferLayout&)     = default;
    BufferLayout(BufferLayout&&) noexcept            = default;
    BufferLayout& operator=(BufferLayout&&) noexcept = default;
    ~BufferLayout()                                  = default;

private:
    constexpr void ComputeOffsetAndStride()
    {
        uint32_t Offset = 0;
        m_Stride        = 0;
        for (auto& E : m_Elements)
        {
            E.Offset = Offset;
            Offset += E.Size;
            m_Stride += E.Size;
            m_Size += E.Size;
        }
    }
    std::vector<BufferElement> m_Elements;
    uint32_t m_Size {0};
    uint32_t m_Stride;
};
/* ====================================
 *            Buffer
 * ====================================
 */
class VT_ENGINE_EXPORT Buffer
{
public:
    virtual void Bind() const   = 0;
    virtual void UnBind() const = 0;

    virtual void SetLayout(BufferLayout)             = 0;
    virtual constexpr BufferLayout GetLayout() const = 0;

    virtual ~Buffer() = default;

protected:
    Buffer() = default;
};
} // namespace VT
