#ifndef SF_BUILT_IN_ANY_HPP
#define SF_BUILT_IN_ANY_HPP

#if __cplusplus >= 201703L && !defined(SF_ANY_SUPPORT_DISABLE)

#include <cstdint> // uint64_t

#include <any> // any

#include <SerializationFixture/Core/Hash.hpp>
#include <SerializationFixture/Core/Serialization.hpp>

#include <SerializationFixture/Dynamic/AnyRegistry.hpp>

// please, use 'sf::serializable' for type any registry before std::any serialization
SERIALIZATION(save, any, std::any)
{
    std::uint64_t hash = any.type().hash_code();
    archive & hash;

    ::sf::dynamic::any_registry.save(archive, any, hash);
}

SERIALIZATION(load, any, std::any)
{
    std::uint64_t hash{};
    archive & hash;

    ::sf::dynamic::any_registry.load(archive, any, hash);
}

#endif // if

#endif // SF_BUILT_IN_ANY_HPP
