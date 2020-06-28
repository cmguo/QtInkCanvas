#include "Internal/Ink/InkSerializedFormat/guidlist.h"
#include "Internal/Ink/InkSerializedFormat/serializationhelper.h"
#include "Internal/Ink/InkSerializedFormat/strokecollectionserializer.h"

INKCANVAS_BEGIN_NAMESPACE

Guid GuidList::Empty;

GuidList::GuidList()
{
}

/// <summary>
/// Adds a guid to the list of Custom Guids if it is not a known guid and is already not
/// in the list of Custom Guids
/// </summary>
/// <param name="guid"></param>
/// <returns></returns>
bool GuidList::Add(Guid const & guid)
{
    // If the guid is not found in the known guids list nor in the custom guid list,
    // add that to the custom guid list
    if (0 == FindTag(guid, true))
    {
        _CustomGuids.Add(guid);
        return true;
    }
    else
        return false;
}


/// <summary>
/// Finds the tag for a Guid based on the list of Known Guids
/// </summary>
/// <param name="guid"></param>
/// <returns></returns>
KnownTagCache::KnownTagIndex GuidList::FindKnownTag(Guid const & guid)
{
    // Find out if the guid is in the known guid table
    for (quint8 iIndex = 0; iIndex < KnownIdCache::OriginalISFIdTableLength; ++iIndex)
        if (guid == KnownIdCache::OriginalISFIdTable[iIndex])
            return static_cast<KnownTagCache::KnownTagIndex>(KnownIdCache::KnownGuidBaseIndex + iIndex);

    // Couldnt find in the known list
    return static_cast<KnownTagCache::KnownTagIndex>(0);
}


/// <summary>
/// Finds the tag for a guid based on the list of Custom Guids
/// </summary>
/// <param name="guid"></param>
/// <returns></returns>
KnownTagCache::KnownTagIndex GuidList::FindCustomTag(Guid const & guid)
{
    int i;

    for (i = 0; i < _CustomGuids.Count(); i++)
    {
        if (guid == _CustomGuids[i])
            return static_cast<KnownTagCache::KnownTagIndex>(KnownIdCache::CustomGuidBaseIndex + i);
    }

    return KnownTagCache::KnownTagIndex::Unknown;
}


/// <summary>
/// Finds a tag corresponding to a guids
/// </summary>
/// <param name="guid"></param>
/// <param name="bFindInKnownListFirst"></param>
/// <returns></returns>
KnownTagCache::KnownTagIndex GuidList::FindTag(Guid const & guid, bool bFindInKnownListFirst)
{
    KnownTagCache::KnownTagIndex tag = KnownTagCache::KnownTagIndex::Unknown;

    if (bFindInKnownListFirst)
    {
        tag = FindKnownTag(guid);
        if (KnownTagCache::KnownTagIndex::Unknown == tag)
            tag = FindCustomTag(guid);
    }
    else
    {
        tag = FindCustomTag(guid);
        if (KnownTagCache::KnownTagIndex::Unknown == tag)
            tag = FindKnownTag(guid);
    }

    return tag;
}


/// <summary>
/// Finds a known guid based on a Tag
/// </summary>
/// <param name="tag"></param>
/// <returns></returns>
Guid const & GuidList::FindKnownGuid(KnownTagCache::KnownTagIndex tag)
{
    if (tag < KnownIdCache::KnownGuidBaseIndex)
    {
        throw std::runtime_error(("Tag is outside of the known guid tag range"));
    }

    // Get the index in the OriginalISFIdTable array first
    quint32 nIndex = (uint)(tag - KnownIdCache::KnownGuidBaseIndex);

    // If invalid, return Guid.Empty
    if (KnownIdCache::OriginalISFIdTableLength <= nIndex)
        return Empty;

    // Otherwise, return the guid
    return KnownIdCache::OriginalISFIdTable[nIndex];
}


/// <summary>
/// Finds a Custom Guid based on a Tag
/// </summary>
/// <param name="tag"></param>
/// <returns></returns>
Guid const & GuidList::FindCustomGuid(KnownTagCache::KnownTagIndex tag)
{
    if ((int)tag < (int)KnownIdCache::CustomGuidBaseIndex)
    {
        throw std::runtime_error(("Tag is outside of the known guid tag range"));
    }

    // Get the index in the OriginalISFIdTable array first
    int nIndex = (int)(tag - KnownIdCache::CustomGuidBaseIndex);

    // If invalid, return Guid.Empty
    if ((0 > nIndex) || (_CustomGuids.Count() <= nIndex))
        return Empty;

    // Otherwise, return the guid
    return _CustomGuids[(int)nIndex];
}


/// <summary>
/// Finds a guid based on Tag
/// </summary>
/// <param name="tag"></param>
/// <returns></returns>
Guid const & GuidList::FindGuid(KnownTagCache::KnownTagIndex tag)
{
    if (tag < (KnownTagCache::KnownTagIndex)KnownIdCache::CustomGuidBaseIndex)
    {
        Guid const & guid = FindKnownGuid(tag);

        if (Empty != guid)
            return guid;

        return FindCustomGuid(tag);
    }
    else
    {
        Guid const & guid = FindCustomGuid(tag);

        if (Empty != guid)
            return guid;

        return FindKnownGuid(tag);
    }
}


/// <summary>
/// Returns the expected size of data if it is a known guid
/// </summary>
/// <param name="guid"></param>
/// <returns></returns>
quint32 GuidList::GetDataSizeIfKnownGuid(Guid const & guid)
{
    for (quint32 i = 0; i < KnownIdCache::OriginalISFIdTableLength; ++i)
    {
        if (guid == KnownIdCache::OriginalISFIdTable[i])
        {
            return KnownIdCache::OriginalISFIdPersistenceSize[i];
        }
    }

    return 0;
}

/// <summary>
/// Serializes the GuidList in the memory stream and returns the size
/// </summary>
/// <param name="stream">If null, calculates the size only</param>
/// <returns></returns>
quint32 GuidList::Save(QIODevice& stream)
{
        // calculate the number of custom guids to persist
        //   custom guids are those which are not reserved in ISF via 'tags'
    quint32 ul = (uint)(_CustomGuids.Count() * sizeof(Guid));

        // if there are no custom guids, then the guid list can be persisted
        //      without any cost ('tags' are freely storeable)
    if (ul == 0)
    {
        return 0;
    }

        // if only the size was requested, return it
    //if (nullptr == stream)
    //{
    //    return (uint)(ul + SerializationHelper::VarSize(ul) + SerializationHelper::VarSize((uint)KnownTagCache::KnownTagIndex::GuidTable));
    //}

        // encode the guid table tag in the output stream
    quint32 cbWrote = SerializationHelper::Encode(stream, (uint)KnownTagCache::KnownTagIndex::GuidTable);

        // encode the size of the guid table
    cbWrote += SerializationHelper::Encode(stream, ul);

        // encode each guid in the table
    for (int i = 0; i < _CustomGuids.Count(); i++)
    {
        Guid const & guid = _CustomGuids[i];

        stream.write(reinterpret_cast<char const*>(&guid.ToByteArray()[0]), sizeof(Guid));
    }

    cbWrote += ul;
    return cbWrote;
}


/// <summary>
/// Deserializes the GuidList from the memory stream
/// </summary>
/// <param name="strm"></param>
/// <param name="size"></param>
/// <returns></returns>
quint32 GuidList::Load(QIODevice& strm, quint32 size)
{
    quint32 cbsize = 0;

    _CustomGuids.Clear();

    quint32 count = size / sizeof(Guid);
    quint8 guids[sizeof(Guid)];

    for (quint32 i = 0; i < count; i++)
    {
        // NTRAID:WINDOWSOS#1622775-2006/04/26-WAYNEZEN,
        // Stream.Read could read less number of bytes than the request. We call ReliableRead that
        // reads the bytes in a loop until all requested bytes are received or reach the end of the stream.
        quint32 bytesRead = StrokeCollectionSerializer::ReliableRead(strm, guids, sizeof(Guid));

        cbsize += bytesRead;
        if ( bytesRead == sizeof(Guid) )
        {
            _CustomGuids.Add(Guid(guids));
        }
        else
        {
            // If Stream.Read cannot return the expected number of bytes, we should break here.
            // The caller - StrokeCollectionSerializer.DecodeRawISF will check our return value.
            // An exception might be thrown if reading is failed.
            break;
        }
    }

    return cbsize;
}

INKCANVAS_END_NAMESPACE
