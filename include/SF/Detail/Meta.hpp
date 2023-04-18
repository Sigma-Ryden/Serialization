#ifndef SF_DETAIL_META_HPP
#define SF_DETAIL_META_HPP

#include <cstddef> // size_t

#include <type_traits>
// is_enum, is_arithmetic, is_array, is_pointer,
// enable_if, is_same, true_type, false_type

#if __cplusplus >= 201703L
#include <any> // any
#endif // if

#include <memory> // shared_ptr
#include <array> // array

#include <SF/Detail/MetaMacro.hpp> // SFVOID

#ifndef SF_MAX_TEMPLATE_DEPTH
    #define SF_MAX_TEMPLATE_DEPTH 256
#endif // SF_MAX_TEMPLATE_DEPTH

namespace sf
{

namespace meta
{

template <bool condition, typename T = void>
using when = typename std::enable_if<condition, T>::type;

template <bool condition>
using require = when<condition, int>;

struct shared_type {};
struct raw_type {};

struct dummy_type
{
#if __cplusplus >= 201703L
    template <typename T, SFREQUIRE(not std::is_same<T, std::any>::value)> operator T();
#else
    template <typename T> operator T();
#endif // if
};

template <std::size_t I>
using dispatch = std::integral_constant<std::size_t, I>;

template <bool condition>
using boolean = std::integral_constant<bool, condition>;

template <typename... Args>
using to_void = void;

template <typename T>
using remove_ptr = typename std::remove_pointer<T>::type;

template <typename T>
using remove_cv = typename std::remove_cv<T>::type;

template <typename T>
using remove_ref = typename std::remove_reference<T>::type;

namespace detail
{

template <typename T, typename = void>
struct deref_implementation { using type = T; };

template <>
struct deref_implementation<void*> { using type = void; };

template <typename T>
struct deref_implementation<std::weak_ptr<T>> { using type = T; };

template <typename T>
struct deref_implementation<T, SFVOID(*std::declval<T>())>
{
    using type = remove_ref<decltype(*std::declval<T>())>;
};

} // namespace detail

template <typename It>
using dereference = typename detail::deref_implementation<It>::type;

template <bool condition, typename if_true, typename if_false>
using conditional = typename std::conditional<condition, if_true, if_false>::type;

template <typename T>
using decay = typename std::decay<T>::type;

template <typename...> struct type_array;

template <> struct type_array<>
{
    template <int I> struct arg { using type = void; };
};

template <typename T, typename... Tn>
struct type_array<T, Tn...>
{
    template <int I, typename overload = void> struct arg
    {
        using type = typename type_array<Tn...>::template arg<I - 1>::type;
    };

    template <typename overload> struct arg<0, overload>
    {
        using type = T;
    };

    template <int I> using type = typename arg<I>::type;
};

template <class...> struct all : std::true_type {};
template <class B1> struct all<B1> : B1 {};
template <class B1, class... Bn>
struct all<B1, Bn...> : conditional<bool(B1::value), all<Bn...>, B1> {};

template <class...> struct one : std::false_type {};
template <class B1> struct one<B1> : B1 {};
template <class B1, class... Bn>
struct one<B1, Bn...> : conditional<bool(B1::value), B1, one<Bn...>> {};

template <class B>
struct negation : boolean<not bool(B::value)> { };

template <typename T, typename... Tn> struct is_same_all : all<std::is_same<T, Tn>...> {};
template <typename T> struct is_same_all<T> : std::true_type {};

// meta
template <typename From, typename To, typename = void>
struct can_static_cast : std::false_type {};

template <typename From, typename To>
struct can_static_cast<From, To,
    SFVOID( static_cast<To*>(std::declval<From*>()) )> : std::true_type {};

namespace detail
{

template <typename, std::size_t N>
struct remove_ptr_n_implementation;

template <typename T>
struct remove_ptr_n_implementation<T*, 1>
{
    using type = T;
};

template <typename T, std::size_t N>
struct remove_ptr_n_implementation<T*, N>
{
    using type = typename remove_ptr_n_implementation<T, N - 1>::type;
};

} // namespace detail

template <typename T, std::size_t N = 1>
using remove_ptr_n = typename detail::remove_ptr_n_implementation<T, N>::type;

namespace detail
{

template <typename T, std::size_t N>
struct pointer_implementation
{
    using type = typename pointer_implementation<T, N - 1>::type*;
};

template <typename T>
struct pointer_implementation<T, 0>
{
    using type = T;
};

} // namespace detail

template <typename T, std::size_t N>
using pointer = typename detail::pointer_implementation<T, N>::type;

 // limited by template depth
template <std::size_t... I>
struct index_sequence
{
    static constexpr std::size_t size() noexcept { return sizeof...(I); }
};

namespace detail
{

template <std::size_t I, std::size_t... In>
struct index_sequence_helper : index_sequence_helper<I - 1, I - 1, In...> {};

template <std::size_t... In>
struct index_sequence_helper<0, In...>
{
    using type = index_sequence<In...>;
};

} // namespace detail

template <std::size_t N>
using make_index_sequence = typename detail::index_sequence_helper<N>::type;

template <typename T, std::size_t I = 0>
auto declval() noexcept -> decltype(std::declval<T>()) { return std::declval<T>(); }

#if __cplusplus >= 201703L
namespace detail
{

template <class C, typename S = index_sequence<>, typename overload = void>
struct aggregate_size_implementation : S {};

template <class C, std::size_t... I>
struct aggregate_size_implementation<C, index_sequence<I...>,
    SFVOID(C{ declval<dummy_type>(), declval<dummy_type, I>()... })>
    : aggregate_size_implementation<C, index_sequence<I..., sizeof...(I)>> {};

template <class T, bool condition = std::is_aggregate<T>::value>
struct aggregate_size
{
    static constexpr auto value = aggregate_size_implementation<T>::size();
};

template <class T>
struct aggregate_size<T, false>
{
    static constexpr auto value = 0;
};

} // namespace detail

template <class T> constexpr std::size_t aggregate_size() noexcept
{
    return detail::aggregate_size<T>::value;
}
#endif // if

constexpr std::size_t max_template_depth() noexcept
{
    return std::size_t(SF_MAX_TEMPLATE_DEPTH);
}

namespace detail
{

template <class T, typename enable = meta::to_void<>>
struct object_value_type_implementation
{
    using type = dummy_type;
};

template <class T>
struct object_value_type_implementation<T, meta::to_void<typename T::value_type>>
{
    using type = typename T::value_type;
};

template <typename T>
struct array_value_type_implementation
{
    using type = dummy_type;
};

template <typename T>
struct array_value_type_implementation<T[]>
{
    using type = T;
};

template <typename T, std::size_t N>
struct array_value_type_implementation<T[N]>
{
    using type = T;
};

template <typename T, typename enable = void>
struct value_type_implementation
{
    using type = dummy_type;
};

template <typename T>
struct value_type_implementation<T, when<std::is_class<T>::value>>
{
    using type = typename object_value_type_implementation<T>::type;
};

template <typename T>
struct value_type_implementation<T, when<std::is_array<T>::value>>
{
    using type = typename array_value_type_implementation<T>::type;
};

} // namespace detail

template <typename T>
using value_type = typename detail::value_type_implementation<T>::type;

template <typename T> constexpr std::size_t pointer_count() noexcept
{
    return std::is_pointer<T>::value
         ? 1 + pointer_count<meta::remove_ptr<T>>()
         : 0;
}

template <typename... Tn> constexpr bool to_false() noexcept { return false; }

template <std::size_t I> constexpr std::size_t with() noexcept
{
    return I;
}

template <std::size_t I = 0, typename B, typename... Bn>
constexpr std::size_t with(B b, Bn... bn) noexcept
{
    return b ? I : with<I + 1>(bn...);
}

template <typename From, typename To> struct is_cast_allowed
    : one<can_static_cast<From, To>, std::is_convertible<From, To>> {};

namespace detail
{

template <typename> struct is_character_implementation : std::false_type {};

template <> struct is_character_implementation<char> : std::true_type {};
template <> struct is_character_implementation<signed char> : std::true_type {};
template <> struct is_character_implementation<unsigned char> : std::true_type {};

template <> struct is_character_implementation<wchar_t> : std::true_type {};
template <> struct is_character_implementation<char16_t> : std::true_type {};
template <> struct is_character_implementation<char32_t> : std::true_type {};

} // namespace detail

template <typename T>
struct is_character : detail::is_character_implementation<typename std::remove_cv<T>::type> {};

template <class T> struct is_compressible : std::is_arithmetic<value_type<T>> {};

template <class Derived, class Base, class... Base_n> struct is_derived_of
    : meta::all<std::is_base_of<Base, Derived>,
                std::is_base_of<Base_n, Derived>...> {};

template <typename T> struct is_void : std::is_same<T, void> {};

template <typename> struct is_std_shared_ptr : std::false_type {};
template <typename T>
struct is_std_shared_ptr<std::shared_ptr<T>> : std::true_type {};

template <typename> struct is_std_array : std::false_type {};
template <typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

template <typename T> struct is_shared_pointer : is_std_shared_ptr<T> {};
template <typename T> struct is_raw_pointer : std::is_pointer<T> {};

template <typename T> struct is_pointer : one<is_shared_pointer<T>, is_raw_pointer<T>> {};

template <typename T> struct is_pointer_to_polymorphic
    : all<is_pointer<T>, std::is_polymorphic<meta::dereference<T>>> {};

template <typename T> struct is_void_pointer : all<is_pointer<T>, is_void<dereference<T>>> {};
template <typename T> struct is_null_pointer : std::is_same<T, std::nullptr_t> {};

template <typename> struct is_function_pointer : std::false_type {};
template <typename Ret, typename... Args>
struct is_function_pointer<Ret (*)(Args...)> : std::true_type {};

template <typename T> struct is_pointer_to_standard_layout :
    all<is_pointer<T>,
        negation<is_void_pointer<T>>,
        negation<is_pointer_to_polymorphic<T>>,
        negation<is_function_pointer<T>>,
        negation<is_null_pointer<T>>,
        negation<std::is_member_pointer<T>>> {};

template <typename T> struct is_serializable_pointer
    : one<is_pointer_to_standard_layout<T>, is_pointer_to_polymorphic<T>> {};

template <typename T> struct is_serializable_raw_pointer
    : all<is_raw_pointer<T>, is_serializable_pointer<T>> {};

template <typename T> struct is_serializable_shared_pointer
    : all<is_shared_pointer<T>, is_serializable_pointer<T>> {};

#if __cplusplus >= 201703L
template <typename T> struct is_aggregate
    : all<std::is_aggregate<T>, negation<is_std_array<T>>, negation<std::is_array<T>>> {};
#endif // if

template <typename T> struct is_unsupported :
    one<is_void_pointer<T>,
        is_function_pointer<T>,
        is_null_pointer<T>,
        std::is_function<T>,
        std::is_member_function_pointer<T>,
        std::is_member_object_pointer<T>,
        std::is_reference<T>> {};

} // namespace meta

} // namespace sf

#endif // SF_DETAIL_META_HPP
