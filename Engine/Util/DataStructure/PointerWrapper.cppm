module;
#include <memory>
export module VT.PointerWrapper;

export namespace VT
{
template <typename T>
using PtrUniq = std::unique_ptr<T>;

template <typename T, typename... Args>
constexpr PtrUniq<T> CreateUniq(Args&&... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
using PtrRef = std::atomic<std::shared_ptr<T>>;

template <typename T, typename... Args>
constexpr PtrRef<T> CreateRef(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}
} // namespace VT
