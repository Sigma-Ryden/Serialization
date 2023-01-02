#ifndef SIRAF_TYPE_REGISTRY_HPP
#define SIRAF_TYPE_REGISTRY_HPP

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

#define CONDITIONAL_TYPE_REGISTRY(...)                                                                  \
    namespace siraf {                                                                                   \
        template <typename T> struct TypeRegistry<T, SIWHEN(__VA_ARGS__)> : std::true_type {};          \
    }

#define TYPE_REGISTRY(...)                                                                              \
    namespace siraf {                                                                                   \
        template <> struct TypeRegistry<__VA_ARGS__> : std::true_type {};                               \
    }

namespace siraf
{

template <typename T, typename enable = void>
struct TypeRegistry : std::false_type {};

namespace meta
{

template <typename T> constexpr bool is_registered() noexcept
{
    return TypeRegistry<T>::value;
}

} // namespace meta

} // namespace siraf

CONDITIONAL_TYPE_REGISTRY(meta::is_arithmetic<T>())
CONDITIONAL_TYPE_REGISTRY(meta::is_enum<T>())
CONDITIONAL_TYPE_REGISTRY(meta::is_array<T>())
CONDITIONAL_TYPE_REGISTRY(meta::is_serializable_raw_pointer<T>())
CONDITIONAL_TYPE_REGISTRY(meta::is_unsupported<T>())

#endif // SIRAF_TYPE_REGISTRY_HPP