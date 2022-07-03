#ifndef SIFAR_SUPPORT_STRING_HPP
#define SIFAR_SUPPORT_STRING_HPP

#include <type_traits> // true_type, false_type

#include <string> // basic_string

#include <Sifar/WriteArchive.hpp>
#include <Sifar/ReadArchive.hpp>

#include <Sifar/TypeRegistry.hpp>

namespace sifar
{

namespace meta
{

template <typename>
struct is_std_basic_string : std::false_type {};

template <typename Char, typename Traits, typename Alloc>
struct is_std_basic_string<std::basic_string<Char, Traits, Alloc>> : std::true_type {};

} // namespace meta

namespace library
{

SERIALIZATION_SAVE_DATA(string, meta::is_std_basic_string<T>::value)
{
    const auto size = string.size();

    archive & size;

    // explicit auto declaration of character without &
    for (auto character : string)
        archive & character;

    return archive;
}

SERIALIZATION_LOAD_DATA(string, meta::is_std_basic_string<T>::value)
{
    using size_type = typename T::size_type;

    size_type size;
    archive & size;

    string.resize(size);

    for(auto& character : string)
        archive & character;

    return archive;
}

} // namespace library

} // namespace sifar

SERIALIZATION_TYPE_REGISTRY_IF(meta::is_std_basic_string<T>::value)

#endif // SIFAR_SUPPORT_STRING_HPP