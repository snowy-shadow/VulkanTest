module;
#include <memory>
export module VT.Util.Datastructure.PointerWrapper;

export namespace VT
{
template <typename T>
using Uniq = std::unique_ptr<T>;

template <typename T, typename... Args>
constexpr Uniq<T> CreateUniq(Args&&... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

// TODO : switch to std::atomic<std::shared_ptr<T>>;
template <typename T>
using Shared = std::shared_ptr<T>;

template <typename T, typename... Args>
constexpr Shared<T> CreateRef(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}
} // namespace VT
