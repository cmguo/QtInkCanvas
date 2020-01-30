#include "Internal/Ink/InkSerializedFormat/metricblock.h"
#include "Internal/Ink/InkSerializedFormat/serializationhelper.h"
#include "Windows/Input/styluspointpropertyinfo.h"

#include <QIODevice>

MetricBlock::MetricBlock()
{

}

// Returns the size required to serialize this instance
uint MetricBlock::Size()
{
    return (_size + SerializationHelper::VarSize(_size));
}

/// <summary>
/// Adds a new metric entry in the existing list of metric entries
/// </summary>
/// <param name="newEntry"></param>
/// <returns></returns>
void MetricBlock::AddMetricEntry(MetricEntry* newEntry)
{
    if (nullptr == newEntry)
    {
        throw std::runtime_error(("MetricEntry cannot be null"));
    }
    if( nullptr == _Entry )
        _Entry = newEntry;
    else
        _Entry->Add(newEntry);  // tack on at the end
    _Count ++;
    _size += newEntry->Size() + SerializationHelper::VarSize(newEntry->Size()) + SerializationHelper::VarSize((uint)newEntry->Tag());
}

/// <summary>
/// Adds a new metric entry in the existing list of metric entries
/// </summary>
/// <param name="property"></param>
/// <param name="tag"></param>
/// <returns></returns>
MetricEntryType MetricBlock::AddMetricEntry(StylusPointPropertyInfo const & property, KnownTagCache::KnownTagIndex tag)
{
    // Create a new metric entry based on the packet information passed.
    MetricEntry* entry = new MetricEntry();
    MetricEntryType type = entry->CreateMetricEntry(property, tag);

    // Don't add this entry to the global list if size is 0, means default metric values!
    if( 0 == entry->Size() )
    {
        return type;
    }

    MetricEntry* start = _Entry;
    if( nullptr == start )
    {
        _Entry = entry;
    }
    else    // tack on data at the end, want to keep x,y at the beginning
    {
        while(start->Next() != nullptr)
        {
            start = start->Next();
        }
        start->SetNext(entry);
    }
    _Count++;
    _size += entry->Size() + SerializationHelper::VarSize(entry->Size()) + SerializationHelper::VarSize((uint)entry->Tag());
    return type;
}

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
uint MetricBlock::Pack(QIODevice& strm)
{
    // Write the size of the Block at the begining of the buffer.
    // But first check the validity of the buffer & its size
    uint cbWrite  = 0;
    // First write the size of the block
    cbWrite = SerializationHelper::Encode(strm, _size);

    // Now write each entry for the block
    MetricEntry* entry = _Entry;
    while( nullptr != entry )
    {
        cbWrite += SerializationHelper::Encode(strm, (uint)entry->Tag());
        cbWrite += SerializationHelper::Encode(strm, entry->Size());
        strm.write((char *)entry->Data(), (int)entry->Size());
        cbWrite += entry->Size();
        entry = entry->Next();
    }
    return cbWrite;
}
//
//

/// <summary>
/// This function compares pMetricColl with the current one. If pMetricColl has more entries apart from the one
/// in the current list with which some of its entries are identical, setType is set as SUPERSET.
/// </summary>
/// <param name="metricColl"></param>
/// <param name="setType"></param>
/// <returns></returns>
bool MetricBlock::CompareMetricBlock( MetricBlock& metricColl, SetType& setType)
{
    //if( nullptr == metricColl )
    //    return false;

    // if both have nullptr entry, implies default metric Block for both of them
    // and it already exists in the list. Return TRUE
    // If only one of the blocks is empty, return FALSE - they cannot be merged
    // because the other block may have customized GUID_X or GUID_Y.

    if (nullptr == GetMetricEntryList())
        return (metricColl.GetMetricEntryList() == nullptr);

    if (nullptr == metricColl.GetMetricEntryList())
        return false;

    // Else compare the entries

    bool  fTagFound = false;
    uint cbLhs = MetricEntryCount();    // No of entries in this block
    uint cbRhs = metricColl.MetricEntryCount();   // No of entries in the block to be compared

    MetricEntry* outside, *inside;
    if( metricColl.MetricEntryCount() <= MetricEntryCount() )
    {
        outside = metricColl.GetMetricEntryList();
        inside  = GetMetricEntryList();
    }
    else
    {
        inside   = metricColl.GetMetricEntryList();
        outside  = GetMetricEntryList();
        setType   = SetType::SuperSet;
    }

    // For each entry in metricColl, search for the same in this Block.
    // If it is found, continue with the next entry of smaller Block.
    while( nullptr != outside )
    {
        fTagFound = false;
        // Always start at the begining of the larger block
        MetricEntry* temp = inside;
        while( nullptr != temp )
        {
            if( outside->Compare(*temp) )
            {
                fTagFound = true;
                break;
            }
            else
                temp = temp->Next();
        }
        if( !fTagFound )
            return false;

        // Found the entry; Continue with the next entry in the outside block
        outside = outside->Next();
    }

    return true;
}
