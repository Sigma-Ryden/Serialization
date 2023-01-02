#ifndef SIRAF_READ_ARCHIVE_HPP
#define SIRAF_READ_ARCHIVE_HPP

#include <cstdint> // uintptr_t
#include <cstddef> // size_t
#include <unordered_map> // unordered_map

#include <Siraf/ArchiveBase.hpp>
#include <Siraf/Dynamic/Registry.hpp>

#include <Siraf/Memory/Memory.hpp>
#include <Siraf/DataTrackBase.hpp>

#include <Siraf/Detail/StaticMessage.hpp>

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

namespace siraf
{

namespace wrapper
{

template <typename InStream>
class InStreamWrapper
{
private:
    InStream& stream_;

public:
    InStreamWrapper(InStream& stream) : stream_(stream) {}

    template <typename T>
    void call(T* data, std::size_t memory_size)
    {
        stream_.read(memory::byte_cast(data), memory_size);
    }

    InStream& get() noexcept { return stream_; }
};

} // namespace wrapper

template <class InStream,
          class StreamWrapper = wrapper::InStreamWrapper<InStream>,
          class Registry = dynamic::ExternRegistry>
class ReadArchive : public core::ArchiveBase
{
    SERIALIZATION_ARCHIVE(ReadArchive)

private:
    template <typename VoidPointer>
    struct TrackData { VoidPointer address = nullptr; };

public:
    using Shared = TrackData<memory::shared_ptr<void>>;
    using Raw = TrackData<memory::raw_ptr<void>>;

public:
    using TrackingKeyType = std::uintptr_t;

    template <typename TrackData>
    using TrackingTable = std::unordered_map<TrackingKeyType, TrackData>;

private:
    StreamWrapper archive_;

    TrackingTable<Shared> track_shared_;
    TrackingTable<Raw> track_raw_;

    Registry registry_;

public:
    ReadArchive(InStream& stream);

    auto stream() noexcept -> StreamWrapper& { return archive_; }

    template <typename TrackType,
              SIREQUIRE(meta::is_track_shared<TrackType>())>
    auto tracking() noexcept -> TrackingTable<Shared>& { return track_shared_; }

    template <typename TrackType,
              SIREQUIRE(meta::is_track_raw<TrackType>())>
    auto tracking() noexcept -> TrackingTable<Raw>& { return track_raw_; }

    auto registry() noexcept -> Registry& { return registry_; }

    template <typename T>
    auto operator>> (T&& data) -> ReadArchive&;

    template <typename T, typename... Tn>
    auto operator() (T& data, Tn&... data_n) -> ReadArchive&;

    auto operator() () -> ReadArchive& { return *this; }
};

template <class InStream>
ReadArchive<InStream> iarchive(InStream& stream)
{
    return { stream };
}

template <template <class, typename...> class StreamWrapper,
          class Registry = dynamic::ExternRegistry,
          class InStream>
ReadArchive<InStream, StreamWrapper<InStream>, Registry> iarchive(InStream& stream)
{
    return { stream };
}

template <class StreamWrapper,
          class Registry = dynamic::ExternRegistry,
          class InStream>
ReadArchive<InStream, StreamWrapper, Registry> iarchive(InStream& stream)
{
    return { stream };
}

template <class InStream, class StreamWrapper, class Registry>
ReadArchive<InStream, StreamWrapper, Registry>::ReadArchive(InStream& stream)
    : archive_{stream}, track_shared_(), track_raw_(), registry_()
{
}

template <class InStream, class StreamWrapper, class Registry>
template <typename T>
auto ReadArchive<InStream, StreamWrapper, Registry>::operator>> (
    T&& data) -> ReadArchive&
{
    return (*this) & data;
}

template <class InStream, class StreamWrapper, class Registry>
template <typename T, typename... Tn>
auto ReadArchive<InStream, StreamWrapper, Registry>::operator() (
    T& data, Tn&... data_n) -> ReadArchive&
{
    (*this) & data;
    return (*this)(data_n...);
}

template <class ReadArchive, typename T,
          SIREQUIRE(meta::is_read_archive<ReadArchive>()
                    and meta::is_unsupported<T>())>
ReadArchive& operator& (ReadArchive& archive, T& unsupported)
{
    static_assert(meta::to_false<T>(),
        "The 'T' is an unsupported type for the 'siraf::ReadArchive'.");

    return archive;
}

template <class ReadArchive, typename T,
          SIREQUIRE(meta::is_read_archive<ReadArchive>()
                    and not meta::is_registered<T>())>
ReadArchive& operator& (ReadArchive& archive, T& unregistered)
{
    static_assert(meta::to_false<T>(),
        STATIC_MESSAGE_UNREGISTERED_TYPE(siraf::ReadArchive, Load));

    return archive;
}

} // namespace siraf

#endif // SIRAF_READ_ARCHIVE_HPP