#ifndef SIFAR_POLYMORPHIC_ARCHIVE_HPP
#define SIFAR_POLYMORPHIC_ARCHIVE_HPP

#include <Sifar/ArchiveBase.hpp>
#include <Sifar/ArchiveTrait.hpp>

#include <Sifar/Access.hpp>

#include <Sifar/Detail/Meta.hpp>
#include <Sifar/Detail/MetaMacro.hpp>

namespace sifar
{

namespace dynamic
{

class PolymorphicArchive
{
public:
    using Archive  = core::ArchiveBase;
    using key_type = core::ArchiveCore::key_type;

    static constexpr key_type max_key = core::ArchiveCore::max_key;

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
        return core::ArchiveTraitKey<Archive>::key != core::ArchiveCore::base_key;
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
    static void try_call(Archive& archive, T& data) { /*pass*/ }

    template <class DerivedArchive, class T,
              SIREQUIRE(is_valid_archive<DerivedArchive>())>
    static void try_call(Archive& archive, T& data)
    {
        auto derived_archive = dynamic_cast<DerivedArchive*>(&archive);
        if (derived_archive == nullptr)
            throw "The read/write archive was not registered.";

        proccess(*derived_archive, data);
    }

    template <class DerivedArchive, class T,
              SIREQUIRE(meta::is_write_archive<DerivedArchive>() and
                        Access::is_save_class<T>())>
    static void proccess(DerivedArchive& archive, T& object)
    {
        Access::save(archive, object);
    }

    template <class DerivedArchive, class T,
              SIREQUIRE(meta::is_read_archive<DerivedArchive>() and
                        Access::is_load_class<T>())>
    static void proccess(DerivedArchive& archive, T& object)
    {
        Access::load(archive, object);
    }

    template <class DerivedArchive, class T,
              SIREQUIRE(meta::is_archive<DerivedArchive>() and
                        meta::is_registered<T>() and
                        not Access::is_save_load_class<T>())>
    static void proccess(DerivedArchive& archive, T& data)
    {
        archive & data;
    }
};

} // namespace dynamic

} // namespace sifar

#endif // SIFAR_POLYMORPHIC_ARCHIVE_HPP
