#ifndef SF_APPLY_FUNCTOR_HPP
#define SF_APPLY_FUNCTOR_HPP

#include <SF/Core/SerializatonBase.hpp>
#include <SF/Core/TypeRegistry.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

namespace sf
{

namespace apply
{

struct ApplyFunctor {};

} // namespace apply

namespace meta
{

template <typename T> struct is_apply_functor : std::is_base_of<apply::ApplyFunctor, T> {};

} // namespace meta

// inline namespace common also used in namespace library
inline namespace common
{

template <typename Archive, typename T,
          typename dT = meta::decay<T>, // T can be lvalue
          SFREQUIRE(meta::all<meta::is_archive<Archive>,
                              meta::is_registered<dT>,
                              meta::is_apply_functor<dT>>::value)>
Archive& operator& (Archive& archive, T&& apply_functor)
{
    apply_functor(archive);
    return archive;
}

} // inline namespace common

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_apply_functor<T>::value)

#endif // SF_APPLY_FUNCTOR_HPP
