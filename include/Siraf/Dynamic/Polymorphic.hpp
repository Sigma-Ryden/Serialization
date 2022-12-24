#ifndef SIRAF_POLYMORPHIC_HPP
#define SIRAF_POLYMORPHIC_HPP

#include <Siraf/Access.hpp> // SERIALIZABLE
#include <Siraf/Dynamic/FactoryTable.hpp> // _CLONEABLE_BODY

#include <Siraf/Dynamic/PolymorphicArchive.hpp>

#define _FACTORY_REGISTRY_DECLARATION(...)                                                              \
    siraf::dynamic::FactoryRegistry<__VA_ARGS__> __registry;

#define _DYNAMIC_SAVE_LOAD_ALIASES(...)                                                                 \
    using clone_type = siraf::dynamic::FactoryTableCore::clone_type;                                    \
    using key_type = siraf::dynamic::FactoryTableCore::key_type;

#define _POLYMORPHIC_ARCHIVE_CALL(function_name)                                                        \
    { siraf::dynamic::PolymorphicArchive::function_name(archive, *this); }

#define _DYNAMIC_SAVE_LOAD_IMPLEMENTATION(...)                                                          \
    void __save(siraf::core::ArchiveBase& archive) _POLYMORPHIC_ARCHIVE_CALL(save)                      \
    void __load(siraf::core::ArchiveBase& archive) _POLYMORPHIC_ARCHIVE_CALL(load)                      \

#define POLYMORPHIC_BODY(...)                                                                           \
    private:                                                                                            \
    _FACTORY_REGISTRY_DECLARATION(__VA_ARGS__)                                                          \
    _DYNAMIC_SAVE_LOAD_ALIASES()                                                                        \
    _DYNAMIC_SAVE_LOAD_IMPLEMENTATION()                                                                 \
    _CLONEABLE_BODY(__VA_ARGS__)                                                                        \
    public:

#ifndef POLYMORPHIC
    #define POLYMORPHIC(...)                                                                            \
        public siraf::dynamic::Polymorphic
#endif // POLYMORPHIC

#ifndef SERIALIZABLE
    #define SERIALIZABLE(...)                                                                           \
        SERIALIZATION_ACCESS(__VA_ARGS__)                                                               \
        POLYMORPHIC_BODY(__VA_ARGS__)
#endif // SERIALIZABLE

namespace siraf
{

namespace dynamic
{

class Polymorphic : public Cloneable
{
public:
    using Archive = siraf::core::ArchiveBase;

private:
    virtual void __save(Archive& archive) = 0;
    virtual void __load(Archive& archive) = 0;
};

} // namespace dynamic

} // namespace siraf

#endif // SIRAF_POLYMORPHIC_HPP
