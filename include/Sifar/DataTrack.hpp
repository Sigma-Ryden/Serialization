#ifndef SIFAR_DATA_TRACK_HPP
#define SIFAR_DATA_TRACK_HPP

#include <memory> // addressof

#include <Sifar/NativeSaveLoad.hpp>

#include <Sifar/Utility.hpp>
#include <Sifar/ApplyFunctor.hpp>

#include <Sifar/Detail/Meta.hpp>

namespace sifar
{

namespace tracking
{

template <class ReadArchive, typename T,
          meta::require<meta::is_read_archive<ReadArchive>()
                        and not meta::is_pointer<T>()> = 0>
void track(ReadArchive& archive, T& data)
{
    using key_type = typename ReadArchive::TrackingTable::key_type;

    key_type key;
    archive & key;

    auto& track_data = archive.tracking()[key];

    if (track_data.is_tracking)
        throw  "the read tracking data is already tracked.";

    auto address = utility::pure(std::addressof(data));

    track_data.address = address;
    track_data.is_tracking = true;

    archive & data;
}

template <class ReadArchive, typename T,
          meta::require<meta::is_read_archive<ReadArchive>()
                        and meta::is_pointer<T>()> = 0>
void track(ReadArchive& archive, T& pointer)
{
    using key_type = typename ReadArchive::TrackingTable::key_type;

    if (pointer != nullptr)
        throw "the read tracking pointer must be initialized to nullptr.";

    key_type key;
    archive & key;

    auto& track_data = archive.tracking()[key];

    if (not track_data.is_tracking)
    {
        archive & pointer; // call the serialization of not tracking pointer

        track_data.address = utility::pure(pointer);
        track_data.is_tracking = true;
    }
    else
    {
        detail::native_load(archive, pointer, track_data.address);
    }
}

template <class WriteArchive, typename T,
          meta::require<meta::is_write_archive<WriteArchive>()
                        and not meta::is_pointer<T>()> = 0>
void track(WriteArchive& archive, T& data)
{
    using key_type = typename WriteArchive::TrackingTable::key_type;

    auto address = utility::pure(std::addressof(data));

    auto key = reinterpret_cast<key_type>(address);

    auto& is_tracking = archive.tracking()[key];

    if (is_tracking)
        throw "the write tracking data is already tracked.";

    is_tracking = true;

    archive & key;
    archive & data;
}

template <class WriteArchive, typename T,
          meta::require<meta::is_write_archive<WriteArchive>()
                        and meta::is_pointer<T>()> = 0>
void track(WriteArchive& archive, T& pointer)
{
    using key_type = typename WriteArchive::TrackingTable::key_type;

    auto key = reinterpret_cast<key_type>(utility::pure(pointer));

    auto& is_tracking = archive.tracking()[key];

    if (not is_tracking)
    {
        archive & key;
        archive & pointer; // call the serialization of not tracking pointer

        is_tracking = true;
    }
    else
    {
        detail::native_save(archive, pointer, key);
    }
}

} // namespace traking

namespace apply
{

template <typename T>
class TrackFunctor : public ApplyFunctor
{
private:
    T& parameter_;

public:
    TrackFunctor(T& parameter) noexcept : parameter_(parameter) {}

    template <typename Archive,
              meta::require<meta::is_archive<Archive>()> = 0>
    void operator() (Archive& archive)
    {
        ::sifar::tracking::track(archive, parameter_);
    }
};

} // namespace apply

namespace tracking
{

template <typename T>
apply::TrackFunctor<T> track(T& parameter) { return { parameter }; }

} // namespace tracking

} // namespace sifar

#endif // SIFAR_DATA_TRACK_HPP