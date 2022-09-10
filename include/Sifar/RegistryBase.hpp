#ifndef SIFAR_REGISTRY_BASE_HPP
#define SIFAR_REGISTRY_BASE_HPP

#include <Sifar/Access.hpp>
#include <Sifar/Utility.hpp>

#include <Sifar/Detail/Meta.hpp>

namespace sifar
{

namespace detail
{

class RegistryBase
{
public:
    using size_type = std::size_t;

protected:
    static constexpr auto key_max_bound = let::u64(-1);

public:
    template <class T>
    static auto key(T& object) noexcept -> decltype(Access::dynamic_key(object))
    {
        return Access::dynamic_key(object);
    }

    template <class T>
    static constexpr auto key() noexcept -> decltype(Access::static_key<T>())
    {
        return Access::static_key<T>();
    }

    template <class Archive, class Pointer,
              meta::require<meta::is_pointer_to_polymorphic<Pointer>()> = 0>
    static size_type save_key(Archive& archive, Pointer& pointer)
    {
        if (pointer == nullptr)
            throw "the write pointer was not allocated.";

        auto id = Access::dynamic_key(*pointer);
        archive & id;

        return id;
    }

    template <class Archive, class Pointer,
              meta::require<meta::is_pointer_to_polymorphic<Pointer>()> = 0>
    static size_type load_key(Archive& archive, Pointer& /*pointer*/)
    {
        using T = meta::deref<Pointer>;
        using key_type = decltype(Access::template static_key<T>());

        key_type id;
        archive & id;

        return id;
    }

protected:
    template <class Derived, class Archive, typename Base,
              meta::require<not meta::is_base_of<meta::deref<Base>, Derived>()
              /*meta::is_void_pointer<Derived*>()*/> = 0>
    static void try_save(Archive& /*archive*/, Base& /*pointer*/)
    {
        throw "serializable type was not registered.";
    }

    template <class Derived, class Archive, typename Base,
              meta::require<meta::is_base_of<meta::deref<Base>, Derived>()
              /*not meta::is_void_pointer<Derived*>()*/> = 0>
    static void try_save(Archive& archive, Base& pointer)
    {
        auto derived = Access::template runtime_cast<Derived*>(pointer);
        archive & (*derived); // will never nullptr
    }

    template <class Derived, class Archive, typename Base,
              meta::require<not meta::is_base_of<meta::deref<Base>, Derived>()
              /*meta::is_void_pointer<Derived*>()*/> = 0>
    static void try_load(Archive& /*archive*/, Base& /*pointer*/)
    {
        throw "serializable type was not registered.";
    }

    template <class Derived, class Archive, typename Base,
              meta::require<meta::is_base_of<meta::deref<Base>, Derived>()
              /*not meta::is_void_pointer<Derived*>()*/> = 0>
    static void try_load(Archive& archive, Base& pointer)
    {
        if (pointer != nullptr)
            throw "the read pointer must be initialized to nullptr.";

        auto derived = new Derived;

        pointer = Access::template cast<Base>(derived);
        archive & (*derived); // will never nullptr
    }

    template <typename Class, typename Pointer,
              meta::require<not Access::is_convertible<Class*, Pointer>()> = 0>
    static void try_assign(Pointer& pointer, void* address) noexcept { /*pass*/ }

    template <typename Class, typename Pointer,
              meta::require<Access::is_convertible<Class*, Pointer>()> = 0>
    static void try_assign(Pointer& pointer, void* address)
    {
        pointer = Access::template cast<Class*>(address);
    }
};

} // namespace detail

} // namespace sifar

#endif // SIFAR_REGISTRY_BASE_HPP
