#ifndef SIFAR_SUPPORT_SET_HPP
#define SIFAR_SUPPORT_SET_HPP

#include <type_traits> // true_type, false_type

#include <set> // set, multiset
#include <unordered_set> // unordered_set, unordered_multiset

#include <utility> // move

#include <Sifar/WriteArchive.hpp>
#include <Sifar/ReadArchive.hpp>

#include <Sifar/TypeRegistry.hpp>

#include <Sifar/Utility.hpp>

#define _SIFAR_IS_STD_MAP_TYPE_META_GENERIC(set_type)                                                   \
    template <typename> struct is_std_##set_type : std::false_type {};                                  \
    template <typename Key, typename Compare, typename Alloc>                                           \
    struct is_std_##set_type<std::set_type<Key, Compare, Alloc>> : std::true_type {};

namespace sifar
{

namespace meta
{

_SIFAR_IS_STD_MAP_TYPE_META_GENERIC(set)
_SIFAR_IS_STD_MAP_TYPE_META_GENERIC(unordered_set)
_SIFAR_IS_STD_MAP_TYPE_META_GENERIC(multiset)
_SIFAR_IS_STD_MAP_TYPE_META_GENERIC(unordered_multiset)

template <class T> constexpr bool is_std_any_set() noexcept
{
    return is_std_set<T>::value
        or is_std_unordered_set<T>::value
        or is_std_multiset<T>::value
        or is_std_unordered_multiset<T>::value;
}

} // namespace meta

inline namespace library
{

CONDITIONAL_SAVE_SERIALIZABLE_TYPE(set, meta::is_std_any_set<T>())
{
    let::u64 size = set.size();
    archive & size;

    for (const auto& item : set)
        archive & item;

    return archive;
}

CONDITIONAL_LOAD_SERIALIZABLE_TYPE(set, meta::is_std_any_set<T>())
{
    using value_type = typename T::value_type;

    let::u64 size;
    archive & size;

    set.clear();

    auto hint = set.begin();
    for (let::u64 i = 0; i < size; ++i)
    {
        value_type item{}; // temp
        archive & item;

        hint = set.emplace_hint(hint, std::move(item));
    }

    return archive;
}

} // inline namespace library

} // namespace sifar

CONDITIONAL_REGISTRY_SERIALIZABLE_TYPE(meta::is_std_any_set<T>())

// clean up
#undef _SIFAR_IS_STD_MAP_TYPE_META_GENERIC

#endif // SIFAR_SUPPORT_SET_HPP
