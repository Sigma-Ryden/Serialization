#ifndef SIRAF_APPLY_FUNCTOR_HPP
#define SIRAF_APPLY_FUNCTOR_HPP

#include <Siraf/TypeRegistry.hpp>

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

namespace siraf
{

namespace apply
{

struct ApplyFunctor {};

} // namespace apply

namespace meta
{

template <typename T> constexpr bool is_apply_functor() noexcept
{
    return meta::is_base_of<apply::ApplyFunctor, T>();
}

} // namespace meta

template <typename Archive, typename T,
          typename U = meta::decay<T>, // T can be lvalue
          SIREQUIRE(meta::is_archive<Archive>() and
                    meta::is_registered<U>() and
                    meta::is_apply_functor<U>())>
Archive& operator& (Archive& archive, T&& apply_functor)
{
    apply_functor(archive);
    return archive;
}

} // namespace siraf

CONDITIONAL_TYPE_REGISTRY(meta::is_apply_functor<T>())

#endif // SIRAF_APPLY_FUNCTOR_HPP