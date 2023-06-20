#ifndef SF_WRITE_ARCHIVE_HPP
#define SF_WRITE_ARCHIVE_HPP

#include <cstdint> // uintptr_t
#include <unordered_map> // unordered_map
#include <utility> // forward

#include <SF/Core/SerializatonBase.hpp>
#include <SF/Core/ArchiveBase.hpp>
#include <SF/Core/TypeRegistry.hpp>

#include <SF/Dynamic/ExternRegistry.hpp>

#include <SF/Core/Memory.hpp>

#include <SF/DataTrackBase.hpp>
#include <SF/HierarchyTrack.hpp>
#include <SF/StreamWrapper.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

namespace sf
{

template <class OutStream,
          class StreamWrapper,
          class Registry = dynamic::ExternRegistry>
class WriteArchive : public core::Archive<core::WriteArchiveType>
{
    SERIALIZATION_ARCHIVE(WriteArchive)

public:
    using TrackingKeyType = std::uintptr_t;
    using TrackingTable = std::unordered_map<TrackingKeyType, bool>;
    using HierarchyTrackingTable = tracking::HierarchyTrack<TrackingKeyType>;

private:
    StreamWrapper archive_;

    TrackingTable track_shared_;
    TrackingTable track_raw_;

    HierarchyTrackingTable track_hierarchy_;

    Registry registry_;

public:    
    WriteArchive(OutStream& stream);

    auto stream() noexcept -> StreamWrapper& { return archive_; }

    template <typename TrackType,
              SFREQUIRE(meta::is_track_shared<TrackType>::value)>
    auto tracking() noexcept -> TrackingTable& { return track_shared_; }

    template <typename TrackType,
              SFREQUIRE(meta::is_track_raw<TrackType>::value)>
    auto tracking() noexcept -> TrackingTable& { return track_raw_; }

    template <typename TrackType,
              SFREQUIRE(meta::is_track_hierarchy<TrackType>::value)>
    auto tracking() noexcept -> HierarchyTrackingTable& { return track_hierarchy_; }

    auto registry() noexcept -> Registry& { return registry_; }

    template <typename T>
    auto operator<< (T&& data) -> WriteArchive&;

    template <typename T, typename... Tn>
    auto operator() (T& data, Tn&... data_n) -> WriteArchive&;

    auto operator() () noexcept -> WriteArchive& { return *this; }
};

// create default WriteArchive<> with wrapper::OByteStream<>
template <class OutStream>
WriteArchive<OutStream, wrapper::OByteStream<OutStream>> oarchive(OutStream& stream)
{
    return { stream };
}

template <template <class, typename...> class StreamWrapper,
          class Registry = dynamic::ExternRegistry,
          class OutStream>
WriteArchive<OutStream, StreamWrapper<OutStream>, Registry> oarchive(OutStream& stream)
{
    return { stream };
}

template <class StreamWrapper,
          class Registry = dynamic::ExternRegistry,
          class OutStream>
WriteArchive<OutStream, StreamWrapper, Registry> oarchive(OutStream& stream)
{
    return { stream };
}

template <class OutStream, class StreamWrapper, class Registry>
WriteArchive<OutStream, StreamWrapper, Registry>::WriteArchive(OutStream& stream)
    : archive_{stream}, track_shared_(), track_raw_(), track_hierarchy_(), registry_()
{
}

template <class OutStream, class StreamWrapper, class Registry>
template <typename T>
auto WriteArchive<OutStream, StreamWrapper, Registry>::operator<< (
    T&& data) -> WriteArchive&
{
    return (*this) & std::forward<T>(data);
}

template <class OutStream, class StreamWrapper, class Registry>
template <typename T, typename... Tn>
auto WriteArchive<OutStream, StreamWrapper, Registry>::operator() (
    T& data, Tn&... data_n) -> WriteArchive&
{
    operator<<(data);
    return operator()(data_n...);
}

template <class Archive, typename T,
          SFREQUIRE(meta::all<meta::is_write_archive<Archive>,
                              meta::is_unsupported<T>>::value)>
Archive& operator& (Archive& archive, T& unsupported)
{
    static_assert(meta::to_false<T>(),
        "The 'T' is an unsupported type for the 'sf::WriteArchive'.");

    return archive;
}

template <class Archive, typename T,
          SFREQUIRE(meta::all<meta::is_write_archive<Archive>,
                              meta::negation<meta::is_registered<T>>>::value)>
Archive& operator& (Archive& archive, T& unregistered)
{
    static_assert(meta::to_false<T>(),
        "The 'T' is an unregistered type for the 'sf::WriteArchive'.");

    return archive;
}

} // namespace sf

#endif // SF_WRITE_ARCHIVE_HPP
