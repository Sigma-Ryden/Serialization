#ifndef SIRAF_CORE_POLYMORPHIC_ARCHIVE_HPP
#define SIRAF_CORE_POLYMORPHIC_ARCHIVE_HPP

#include <Siraf/Core/Access.hpp>
#include <Siraf/Core/Serialization.hpp>
#include <Siraf/Core/TypeRegistry.hpp>

#include <Siraf/Core/ArchiveBase.hpp>
#include <Siraf/Core/ArchiveTrait.hpp>

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

namespace siraf
{

namespace core
{

class PolymorphicArchive
{
public:
    using Archive  = core::ArchiveBase;
    using key_type = core::ArchiveBase::key_type;

    static constexpr key_type max_key = core::ArchiveTraitBase::max_key;

public:
    template <class T> static void save(Archive& archive, T& data)
    {
        call<core::WriteArchiveTrait>(archive, data);
    }

    template <class T> static void load(Archive& archive, T& data)
    {
        call<core::ReadArchiveTrait>(archive, data);
    }

private:
    template <class Archive> static constexpr bool is_valid_archive() noexcept
    {
        return core::ArchiveTraitKey<Archive>::key != core::ArchiveTraitBase::base_key;
    }

private:
    template <template <key_type> class ArchiveTrait,
              key_type Key, class T, SIREQUIRE(Key == max_key)>
    static void call(Archive& archive, T& data)
    {
        throw "The read/write archive has invalid type key.";
    }

    template <template <key_type> class ArchiveTrait,
              key_type Key = 0, class T, SIREQUIRE(Key < max_key)>
    static void call(Archive& archive, T& data)
    {
        using DerivedArchive = typename ArchiveTrait<Key>::type;

        if (core::ArchiveTraitKey<DerivedArchive>::key == archive.trait())
            return try_call<DerivedArchive>(archive, data);

        call<ArchiveTrait, Key + 1>(archive, data);
    }

    template <class DerivedArchive, class T,
              SIREQUIRE(not is_valid_archive<DerivedArchive>())>
    static void try_call(Archive& archive, T& data)
    {
        throw "The read/write archive was not registered.";
    }

    template <class DerivedArchive, class T,
              SIREQUIRE(is_valid_archive<DerivedArchive>())>
    static void try_call(Archive& archive, T& data)
    {
        auto derived_archive = dynamic_cast<DerivedArchive*>(&archive);

    #ifdef SIRAF_DEBUG
        if (derived_archive == nullptr)
            throw "The read/write archive was registered incorrect.";
    #endif // SIRAF_DEBUG

        proccess(*derived_archive, data);
    }

    template <class DerivedArchive, class T,
              SIREQUIRE(meta::is_write_archive<DerivedArchive>() and
                        ::Serialization::has_save_mode<T>())>
    static void proccess(DerivedArchive& archive, T& object)
    {
        core::Access::save(archive, object);
    }

    template <class DerivedArchive, class T,
              SIREQUIRE(meta::is_read_archive<DerivedArchive>() and
                        ::Serialization::has_load_mode<T>())>
    static void proccess(DerivedArchive& archive, T& object)
    {
        core::Access::load(archive, object);
    }

    template <class DerivedArchive, class T,
              SIREQUIRE(meta::is_archive<DerivedArchive>() and
                        not ::Serialization::has_save_mode<T>() and
                        not ::Serialization::has_load_mode<T>())>
    static void proccess(DerivedArchive& archive, T& data)
    {
        process_data(archive, data);
    }

    template <class DerivedArchive, class T,
              SIREQUIRE(meta::is_registered<T>())>
    static void process_data(DerivedArchive& archive, T& data)
    {
        archive & data;
    }

    template <class DerivedArchive, class T,
              SIREQUIRE(not meta::is_registered<T>())>
    static void process_data(DerivedArchive& archive, T& data)
    {
        throw "The 'T' type is unregistered.";
    }
};

template <class Archive, typename T,
          SIREQUIRE(meta::is_base_archive<Archive>())>
Archive& operator<< (Archive& archive, T& data)
{
    PolymorphicArchive::save(archive, data);
    return archive;
}

template <class Archive, typename T,
          SIREQUIRE(meta::is_base_archive<Archive>())>
Archive& operator>> (Archive& archive, T& data)
{
    PolymorphicArchive::load(archive, data);
    return archive;
}

} // namespace core

} // namespace siraf

#endif // SIRAF_CORE_POLYMORPHIC_ARCHIVE_HPP