#ifndef SF_BUILT_IN_ANY_HPP
#define SF_BUILT_IN_ANY_HPP

#if !defined(SF_ANY_SUPPORT_DISABLE)

#include <cstdint> // uint64_t

#include <any> // any

#include <SerializationFixture/Core/Hash.hpp>
#include <SerializationFixture/Core/Serialization.hpp>

#include <SerializationFixture/Dynamic/AnyRegistry.hpp>

// please, use 'sf::serializable' for type any registry before std::any serialization
SERIALIZABLE_DECLARATION(std::any)
SERIALIZABLE_DECLARATION_INIT()

#endif // if

#endif // SF_BUILT_IN_ANY_HPP