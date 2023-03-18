// You MUST NOT include other files in this file, than meta and macros.
#ifndef SIRAF_CORE_ACCESS_HPP
#define SIRAF_CORE_ACCESS_HPP

#include <Siraf/Core/Serialization.hpp>

#include <Siraf/Core/PolymorphicTrait.hpp>
#include <Siraf/Core/ArchiveBase.hpp>

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

#define SERIALIZATION_ACCESS(...)                                                                       \
    friend class ::Serialization;                                                                       \
    friend class ::siraf::core::Access;

#define _SIRAF_HAS_PROPERTY_HELPER(extern_name, inner_name)                                             \
    template <typename C, typename = void>                                                              \
    struct has_##extern_name : std::false_type {};                                                      \
    template <typename C>                                                                               \
    struct has_##extern_name<C, ::siraf::meta::to_void<decltype(&C::inner_name)>>                       \
        : std::true_type {};

namespace siraf
{

namespace core
{

class Access
{
private:
    _SIRAF_HAS_PROPERTY_HELPER(save, __save)
    _SIRAF_HAS_PROPERTY_HELPER(load, __load)

    _SIRAF_HAS_PROPERTY_HELPER(static_trait, __static_trait)
    _SIRAF_HAS_PROPERTY_HELPER(trait, __trait)

public:
    template <class T>
    static constexpr bool is_dynamic_save_class() noexcept
    {
        return has_save<T>::value;
    }

    template <class T>
    static constexpr bool is_dynamic_load_class() noexcept
    {
        return has_load<T>::value;
    }

    template <class T>
    static constexpr bool is_registered_class() noexcept
    {
        return has_static_trait<T>::value and has_trait<T>::value;
    }

private:
    template <typename From, typename To, typename = void>
    struct can_static_cast_impl : std::false_type {};

    template <typename From, typename To>
    struct can_static_cast_impl<From, To,
        meta::to_void<decltype( static_cast<To>(std::declval<From>()) )>>
    : std::true_type {};

    template <class, class> static std::false_type try_cast(...);
    template <class From, class To> static auto try_cast(int) -> decltype
    (
        (void) std::declval<void(&)(To)>()(std::declval<From>()),
        std::true_type{}
    );

public:
    template <class From, class To>
    static constexpr bool can_static_cast() noexcept
    {
        return can_static_cast_impl<From, To>::value;
    }

    template <class From, class To>
    static constexpr bool is_convertible() noexcept
    {
        return (meta::is_returnable<To>() and decltype(try_cast<From, To>(0))::value)
            or (meta::is_same_all<void, From, To>());
    }

public:
    template <typename Archive, typename T>
    static void save(Archive& archive, T& data)
    {
        ::Serialization::SaveMode<T>::call(archive, data);
    }

    template <typename Archive, typename T>
    static void load(Archive& archive, T& data)
    {
        ::Serialization::LoadMode<T>::call(archive, data);
    }

    template <typename Pointer, typename T = meta::dereference<Pointer>,
              SIREQUIRE(is_dynamic_save_class<T>() and ::Serialization::has_save_mode<T>())>
    static void dynamic_save(core::ArchiveBase& archive, Pointer& object)
    {
        object->__save(archive);
    }

    template <typename Pointer, typename T = meta::dereference<Pointer>,
              SIREQUIRE(is_dynamic_load_class<T>() and ::Serialization::has_load_mode<T>())>
    static void dynamic_load(core::ArchiveBase& archive, Pointer& object)
    {
        object->__load(archive);
    }

    template <typename Base, class Archive, typename Derived,
              SIREQUIRE(meta::is_archive<Archive>() and
                        meta::is_base_of<Base, Derived>())>
    static void serialize_base(Archive& archive, Derived& object)
    {
        archive & cast<Base&>(object);
    }

    template <class T, SIREQUIRE(is_registered_class<T>())>
    static auto trait(T& object) noexcept -> decltype(object.__trait())
    {
        return object.__trait();
    }

    template <class T, SIREQUIRE(is_registered_class<T>())>
    static constexpr PolymorphicTraitBase::key_type static_trait() noexcept
    {
        return T::__static_trait();
    }

    template <class T, SIREQUIRE(is_registered_class<T>())>
#ifdef SIRAF_EXPORT_POLYMORPHIC_DISABLE
    static constexpr PolymorphicTraitBase::key_type trait() noexcept
#else
    static PolymorphicTraitBase::key_type trait() noexcept
#endif // SIRAF_EXPORT_POLYMORPHIC_DISABLE
    {
        constexpr auto trait_key = PolymorphicTraitKey<T>::key;

        return trait_key == PolymorphicTraitBase::base_key
             ? static_trait<T>()
             : trait_key;
    }

private:
    template <class To, class From>
    static To cast(From& from) noexcept
    {
        return static_cast<To>(from);
    }

    template <class To, class From>
    static To runtime_cast(From& from) // maybe throw exception
    {
        return dynamic_cast<To>(from);
    }
};

} // namespace core

} // namespace siraf

// clean up
#undef _SIRAF_HAS_PROPERTY_HELPER

#endif // SIRAF_CORE_ACCESS_HPP