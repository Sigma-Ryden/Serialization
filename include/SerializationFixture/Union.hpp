#ifndef SF_UNION_HPP
#define SF_UNION_HPP

#include <SerializationFixture/Core/Serialization.hpp>
#include <SerializationFixture/Binary.hpp>

CONDITIONAL_SERIALIZABLE_DECLARATION(std::is_union<S>::value)
SERIALIZABLE_DECLARATION_INIT()

CONDITIONAL_SERIALIZABLE(saveload, data, std::is_union<S>::value)
    SERIALIZATION
    (
        ::sf::binary(archive, data);
    )
SERIALIZABLE_INIT()

#endif // SF_UNION_HPP