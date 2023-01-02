#ifndef SIRAF_SUPPORT_VECTOR_HPP
#define SIRAF_SUPPORT_VECTOR_HPP

#include <type_traits> // true_type, false_type

#include <vector> // vector

#include <Siraf/ExternSerialization.hpp>
#include <Siraf/Compress.hpp>
#include <Siraf/TypeRegistry.hpp>

#include <Siraf/Utility.hpp>

namespace siraf
{

namespace meta
{

template <typename> struct is_std_vector : std::false_type {};
template <typename T, typename Alloc>
struct is_std_vector<std::vector<T, Alloc>> : std::true_type {};

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, vector, meta::is_std_vector<T>::value)
{
    let::u64 size = vector.size();
    archive & size;

    compress::zip(archive, vector);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, vector, meta::is_std_vector<T>::value)
{
    let::u64 size;
    archive & size;

    vector.resize(size);
    compress::zip(archive, vector);

    return archive;
}

} // inline namespace library

} // namespace siraf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_vector<T>::value)

#endif // SIRAF_SUPPORT_VECTOR_HPP