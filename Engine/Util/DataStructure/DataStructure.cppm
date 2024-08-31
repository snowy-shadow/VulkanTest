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
    DependencyWrapper(
        T Item, std::function<void()> Dtor = []() {}) :
        m_Data(Item), m_Dtor(Dtor)
    {
    }
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

struct NoneT {};
static inline constexpr NoneT None = NoneT {};

template <typename T>
struct Optional
{
private:
    T m_value        = {};
    bool m_has_value = {};

public:
    Optional() : m_value {}, m_has_value {false} {}
    Optional(Optional<T> const&) = default;
    Optional(T const& v) : m_value {v}, m_has_value {true} {}
    Optional(NoneT const&) : m_value {}, m_has_value {} {}
    Optional<T>& operator=(Optional<T> const&) = default;
    Optional<T>& operator=(T const& v)
    {
        this->m_value     = v;
        this->m_has_value = true;
        return *this;
    }
    Optional<T>& operator=(NoneT const&)
    {
        this->m_value     = {};
        this->m_has_value = {};
        return *this;
    }

    [[nodiscard]] auto has_value() const -> bool { return this->m_has_value; }

    [[nodiscard]] auto value() -> T& { return this->m_value; }

    [[nodiscard]] auto value() const -> T const& { return this->m_value; }

    [[nodiscard]] auto value_or(T const& v) const -> T const& { return has_value() ? value() : v; }
};
} // namespace VT
