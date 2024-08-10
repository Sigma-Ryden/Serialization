#ifndef SF_DYNAMIC_ANY_REGISTRY_HPP
#define SF_DYNAMIC_ANY_REGISTRY_HPP

#if __cplusplus >= 201703L && !defined(SF_ANY_SUPPORT_DISABLE)

#include <unordered_map> // unordered_map
#include <any> // any

#include <SF/Core/PolymorphicArchive.hpp>
#include <SF/Core/TypeCore.hpp>

#include <SF/Detail/Meta.hpp>

namespace sf
{

namespace dynamic
{

class any_registry_t
{
public:
    struct any_proxy_t
    {
        // we use raw function ptr instead std::function to reach perfomance
        void(*save)(core::ioarchive_t&, std::any&) = nullptr;
        void(*load)(core::ioarchive_t&, std::any&) = nullptr;
    };

public:
    std::unordered_map<let::u64, any_proxy_t> all;

private:
    any_registry_t() = default;
    any_registry_t(const any_registry_t&) = delete;
    any_registry_t& operator=(const any_registry_t&) = delete;

public:
    static any_registry_t& instance() noexcept
    {
        static any_registry_t self;
        return self;
    }

    template <typename T>
    void add()
    {
        static bool lock = false; if (lock) return;
        lock = true;

        any_proxy_t proxy;

        proxy.save = [](core::ioarchive_t& archive, std::any& any)
        {
            archive << std::any_cast<T&>(any);
        };

        proxy.load = [](core::ioarchive_t& archive, std::any& any)
        {
            any.emplace<T>();
            archive >> std::any_cast<T&>(any);
        };

        all.emplace(typeid(T).hash_code(), proxy);
    }

public:
    void save(core::ioarchive_t& archive, std::any& any, let::u64 hash)
    {
        all.at(hash).save(archive, any);
    }

    void load(core::ioarchive_t& archive, std::any& any, let::u64 hash)
    {
        all.at(hash).load(archive, any);
    }
};

} // namespace dynamic

} // namespace sf

#endif // if

#endif // SF_DYNAMIC_ANY_REGISTRY_HPP
