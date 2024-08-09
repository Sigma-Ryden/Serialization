#ifndef SF_DYNAMIC_INSTANTIABLE_TRAITS_HPP
#define SF_DYNAMIC_INSTANTIABLE_TRAITS_HPP

#include <SF/Core/Hash.hpp>

#define EXPORT_INSTANTIABLE(...)                                                                        \
    template <> struct xxsf_instantiable_fixture<__VA_ARGS__> {                                         \
        static bool fixture() {                                                                         \
            return ::sf::dynamic::instantiable_registry_t::instance().fixture<__VA_ARGS__>();           \
        }                                                                                               \
    };

#define EXPORT_INSTANTIABLE_KEY(instantiable_key, ...)                                                  \
    template <> struct xxsf_instantiable_traits<__VA_ARGS__> {                                          \
        static ::xxsf_instantiable_traits_key_type key() {                                              \
            return instantiable_key;                                                                    \
        }                                                                                               \
    };                                                                                                  \
    template <> struct xxsf_instantiable_fixture<__VA_ARGS__> {                                         \
        static bool fixture() {                                                                         \
            return ::sf::dynamic::instantiable_registry_t::instance().fixture<__VA_ARGS__>();           \
        }                                                                                               \
    };

using xxsf_instantiable_traits_key_type = SF_STATIC_HASH_KEY_T;
static constexpr auto xxsf_instantiable_traits_base_key = xxsf_instantiable_traits_key_type(-1);

template <class InstantiableType>
struct xxsf_instantiable_fixture
{
    static bool fixture() { return false; }
};
// TODO: auto run
template <class InstantiableType>
struct xxsf_instantiable_traits
{
    static ::xxsf_instantiable_traits_key_type key() { return SF_TYPE_HASH(InstantiableType); }
};

#endif // SF_DYNAMIC_INSTANTIABLE_TRAITS_HPP