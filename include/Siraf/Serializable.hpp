#ifndef SIRAF_SERIALIZABLE_HPP
#define SIRAF_SERIALIZABLE_HPP

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

#define SERIALIZATION(mode, ...)                                                                        \
    template <> struct Serialization::mode<__VA_ARGS__> {                                               \
        template <typename Archive>                                                                     \
        static void call(Archive& archive, __VA_ARGS__& self);                                          \
    };                                                                                                  \
    template <typename Archive>                                                                         \
    void Serialization::mode<__VA_ARGS__>::call(Archive& archive, __VA_ARGS__& self)

#define CONDITIONAL_SERIALIZATION(mode, ...)                                                            \
    template <typename T>                                                                               \
    struct Serialization::mode<T, SIWHEN(__VA_ARGS__)> {                                                \
        template <typename Archive>                                                                     \
        static void call(Archive& archive, T& self);                                                    \
    };                                                                                                  \
    template <typename T>                                                                               \
    template <typename Archive>                                                                         \
    void Serialization::mode<T, SIWHEN(__VA_ARGS__)>::call(Archive& archive, T& self)

class Serialization
{
public:
    template <typename T, typename enable = void>
    struct SaveLoad { static constexpr bool pure = true; };

    template <typename T, typename enable = void>
    struct Save { static constexpr bool pure = true; };

    template <typename T, typename enable = void>
    struct Load { static constexpr bool pure = true; };

public:
    struct SaveFail
    {
        template <typename Archive, typename T>
        static void call(Archive& archive, T& self)
        {
            throw "The 'T' type cannot be saved.";
        }
    };

    struct LoadFail
    {
        template <typename Archive, typename T>
        static void call(Archive& archive, T& self)
        {
            throw "The 'T' type cannot be loaded.";
        }
    };
};

#endif // SIRAF_SERIALIZABLE_HPP
