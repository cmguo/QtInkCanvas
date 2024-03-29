#ifndef METRICBLOCK_H
#define METRICBLOCK_H

#include "Internal/Ink/InkSerializedFormat/metricentry.h"

class QIODevice;

INKCANVAS_BEGIN_NAMESPACE

class MetricEntry;

/// <summary>
/// CMetricBlock owns CMetricEntry which is created based on the Packet Description of the stroke. It also
/// stores the pointer of the next Block. This is not used in the context of a stroke but is used in the
/// context of WispInk. Wispink forms a linked list based on the CMetricBlocks of all the strokes.
/// </summary>

class MetricBlock
{
    MetricEntry* _Entry = nullptr;
    uint        _Count = 0;
    uint        _size = 0;

public:
    /// <summary>
    /// Constructor
    /// </summary>
    MetricBlock();

    /// <summary>
    /// Gets the MetricEntry list associated with this instance
    /// </summary>
    /// <returns></returns>
    MetricEntry* GetMetricEntryList()
    {
        return _Entry;
    }

    /// <summary>
    /// Gets the count of MetricEntry for this instance
    /// </summary>
    uint MetricEntryCount()
    {
        return _Count;
    }

    // Returns the size required to serialize this instance
    uint Size();

    /// <summary>
    /// Adds a new metric entry in the existing list of metric entries
    /// </summary>
    /// <param name="newEntry"></param>
    /// <returns></returns>
    void AddMetricEntry(MetricEntry* newEntry);

    /// <summary>
    /// Adds a new metric entry in the existing list of metric entries
    /// </summary>
    /// <param name="property"></param>
    /// <param name="tag"></param>
    /// <returns></returns>
    MetricEntryType AddMetricEntry(StylusPointPropertyInfo const & property, KnownTagCache::KnownTagIndex tag);

    /// <summary>
    /// This function Packs the data in the buffer provided. The
    /// function is being called during the save loop and caller
    /// must call GetSize for the block before calling this function.
    /// The buffer must be preallocated and buffer size must be at
    /// least the size of the block.
    /// On return cbBuffer contains the size of the data written.
    /// Called only by BuildMetricTable funtion
    /// </summary>
    /// <param name="strm"></param>
    /// <returns></returns>
    uint Pack(QIODevice& strm);
    //
    //

    /// <summary>
    /// This function compares pMetricColl with the current one. If pMetricColl has more entries apart from the one
    /// in the current list with which some of its entries are identical, setType is set as SUPERSET.
    /// </summary>
    /// <param name="metricColl"></param>
    /// <param name="setType"></param>
    /// <returns></returns>
    bool CompareMetricBlock( MetricBlock& metricColl, SetType& setType);
};

INKCANVAS_END_NAMESPACE

#endif // METRICBLOCK_H
