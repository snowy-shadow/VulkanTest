module;
#include <functional>
#include <memory>

export module VT.Util.DataStructure;

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
constexpr Shared<T> CreateShared(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename T, typename... Dependencies>
class DependencyWrapper
{
public:
    DependencyWrapper(T Item, std::function<void()> Dtor = []() {}) : m_Data(Item), m_Dtor(Dtor) {}
    DependencyWrapper(T Item, std::function<void()> Dtor, Shared<Dependencies>... Depends) :
        m_Data(Item), m_Dtor(Dtor), m_Dependencies(std::forward<Shared<Dependencies>>(Depends)...)
    {
    }

    template <typename U>
    void AddDependency(Shared<U> Item)
    {
        std::get<Shared<U>>(m_Dependencies) = Item;
    }

    T& Get() const noexcept { return m_Data; }

    ~DependencyWrapper() { m_Dtor(m_Data); }

private:
    std::tuple<Shared<Dependencies>...> m_Dependencies;
    std::function<void()> m_Dtor;
    T m_Data;
};
} // namespace VT
