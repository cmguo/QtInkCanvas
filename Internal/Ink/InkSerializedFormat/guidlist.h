#ifndef GUIDLIST_H
#define GUIDLIST_H

#include "InkCanvas_global.h"
#include "Internal/Ink/InkSerializedFormat/isftagandguidcache.h"
#include "Collections/Generic/list.h"
#include "guid.h"

#define OLD_ISF 0

class QIODevice;

INKCANVAS_BEGIN_NAMESPACE

/// <summary>
/// Summary description for GuidTagList.
/// </summary>
class GuidList
{
private:
    List<Guid> _CustomGuids;


public:
    static Guid Empty;

    GuidList();

    /// <summary>
    /// Adds a guid to the list of Custom Guids if it is not a known guid and is already not
    /// in the list of Custom Guids
    /// </summary>
    /// <param name="guid"></param>
    /// <returns></returns>
    bool Add(Guid const & guid);


    /// <summary>
    /// Finds the tag for a Guid based on the list of Known Guids
    /// </summary>
    /// <param name="guid"></param>
    /// <returns></returns>
    static KnownTagCache::KnownTagIndex FindKnownTag(Guid const & guid);


    /// <summary>
    /// Finds the tag for a guid based on the list of Custom Guids
    /// </summary>
    /// <param name="guid"></param>
    /// <returns></returns>
    KnownTagCache::KnownTagIndex FindCustomTag(Guid const & guid);


    /// <summary>
    /// Finds a tag corresponding to a guids
    /// </summary>
    /// <param name="guid"></param>
    /// <param name="bFindInKnownListFirst"></param>
    /// <returns></returns>
    KnownTagCache::KnownTagIndex FindTag(Guid const & guid, bool bFindInKnownListFirst);


    /// <summary>
    /// Finds a known guid based on a Tag
    /// </summary>
    /// <param name="tag"></param>
    /// <returns></returns>
    static Guid const & FindKnownGuid(KnownTagCache::KnownTagIndex tag);


    /// <summary>
    /// Finds a Custom Guid based on a Tag
    /// </summary>
    /// <param name="tag"></param>
    /// <returns></returns>
    Guid const & FindCustomGuid(KnownTagCache::KnownTagIndex tag);


    /// <summary>
    /// Finds a guid based on Tag
    /// </summary>
    /// <param name="tag"></param>
    /// <returns></returns>
    Guid const & FindGuid(KnownTagCache::KnownTagIndex tag);


    /// <summary>
    /// Returns the expected size of data if it is a known guid
    /// </summary>
    /// <param name="guid"></param>
    /// <returns></returns>
    static quint32 GetDataSizeIfKnownGuid(Guid const & guid);

    /// <summary>
    /// Serializes the GuidList in the memory stream and returns the size
    /// </summary>
    /// <param name="stream">If null, calculates the size only</param>
    /// <returns></returns>
    quint32 Save(QIODevice& stream);


    /// <summary>
    /// Deserializes the GuidList from the memory stream
    /// </summary>
    /// <param name="strm"></param>
    /// <param name="size"></param>
    /// <returns></returns>
    quint32 Load(QIODevice& strm, quint32 size);
};

INKCANVAS_END_NAMESPACE

#endif // GUIDLIST_H
