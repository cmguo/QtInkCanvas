#ifndef WINDOWS_INPUT_STYLUSPOINTDESCRIPTION_H
#define WINDOWS_INPUT_STYLUSPOINTDESCRIPTION_H

#include "Windows/Input/styluspointpropertyinfo.h"

#include "Collections/Generic/list.h"
#include "Collections/Generic/array.h"
#include "sharedptr.h"

#include <iterator>

// namespace System.Windows.Input
INKCANVAS_BEGIN_NAMESPACE

class StylusPointDescription
{
public:
    /// <summary>
    /// statics for our magic numbers
    /// </summary>
    static constexpr int RequiredCountOfProperties = 3;
    static constexpr int RequiredXIndex = 0;
    static constexpr int RequiredYIndex = 1;
    static constexpr int RequiredPressureIndex = 2;
    static constexpr int MaximumButtonCount = 31;

    int _buttonCount = 0;
    int _originalPressureIndex = RequiredPressureIndex;
    List<StylusPointPropertyInfo>   _stylusPointPropertyInfos;

    /// <summary>
    /// StylusPointDescription
    /// </summary>
    StylusPointDescription();

    /// <summary>
    /// StylusPointDescription
    /// </summary>
    StylusPointDescription(List<StylusPointPropertyInfo> const & stylusPointPropertyInfos);

    /// <summary>
    /// StylusPointDescription
    /// </summary>
    /// <param name="stylusPointPropertyInfos">stylusPointPropertyInfos
    /// <param name="originalPressureIndex">originalPressureIndex - does the digitizer really support pressure?  If so, the index this was at
    StylusPointDescription(List<StylusPointPropertyInfo> const & stylusPointPropertyInfos, int originalPressureIndex);
    /// <summary>
    /// HasProperty
    /// </summary>
    /// <param name="stylusPointProperty">stylusPointProperty
    bool HasProperty(StylusPointProperty const & stylusPointProperty) const;

    /// <summary>
    /// The count of properties this StylusPointDescription contains
    /// </summary>
    int PropertyCount() const;

    /// <summary>
    /// GetProperty
    /// </summary>
    /// <param name="stylusPointProperty">stylusPointProperty
    StylusPointPropertyInfo GetPropertyInfo(StylusPointProperty const & stylusPointProperty) const;

    /// <summary>
    /// GetPropertyInfo
    /// </summary>
    /// <param name="Guid">Guid
    StylusPointPropertyInfo GetPropertyInfo(Guid const & Guid) const;

    /// <summary>
    /// Returns the index of the given StylusPointProperty by ID, or -1 if none is found
    /// </summary>
    int GetPropertyIndex(Guid Guid) const;

    /// <summary>
    /// GetStylusPointProperties
    /// </summary>
    List<StylusPointPropertyInfo> GetStylusPointProperties() const;

    /// <summary>
    /// GetStylusPointPropertyIdss
    /// </summary>
    Array<Guid> GetStylusPointPropertyIds() const;

    /// <summary>
    /// helper for determining how many ints in a raw int array
    /// correspond to one point we get from the input system
    /// </summary>
    int GetInputArrayLengthPerPoint() const;

    /// <summary>
    /// helper for determining how many members a StylusPoint's
    /// int[] should be for additional data
    /// </summary>
    int GetExpectedAdditionalDataCount() const;

    /// <summary>
    /// helper for determining how many ints in a raw int array
    /// correspond to one point when saving to himetric
    /// </summary>
    /// <returns></returns>
    int GetOutputArrayLengthPerPoint() const;

    /// <summary>
    /// helper for determining how many buttons are present
    /// </summary>
    int ButtonCount() const;

    /// <summary>
    /// helper for determining what bit position the button is at
    /// </summary>
    int GetButtonBitPosition(StylusPointProperty buttonProperty) const;

    /// <summary>
    /// ContainsTruePressure - true if this StylusPointDescription was instanced
    /// by a TabletDevice or by ISF serialization that contains NormalPressure
    /// </summary>
    bool ContainsTruePressure() const;

    /// <summary>
    /// helper to determine the original pressure index
    /// </summary>
    int OriginalPressureIndex() const;

    /// <summary>
    /// Returns true if the two StylusPointDescriptions have the same StylusPointProperties.  Metrics are ignored.
    /// </summary>
    /// <param name="stylusPointDescription1">stylusPointDescription1
    /// <param name="stylusPointDescription2">stylusPointDescription2
    static bool AreCompatible(SharedPointer<StylusPointDescription> stylusPointDescription1, SharedPointer<StylusPointDescription> stylusPointDescription2);

    /// <summary>
    /// Returns a new StylusPointDescription with the common StylusPointProperties from both
    /// </summary>
    /// <param name="stylusPointDescription">stylusPointDescription
    /// <param name="stylusPointDescriptionPreserveInfo">stylusPointDescriptionPreserveInfo
    /// <remarks>The StylusPointProperties from stylusPointDescriptionPreserveInfo will be returned in the new StylusPointDescription</remarks>
    static SharedPointer<StylusPointDescription> GetCommonDescription(SharedPointer<StylusPointDescription> stylusPointDescription,
                                                         SharedPointer<StylusPointDescription> stylusPointDescriptionPreserveInfo);

    /// <summary>
    /// Returns true if this StylusPointDescription is a subset
    /// of the StylusPointDescription passed in
    /// </summary>
    /// <param name="stylusPointDescriptionSuperset">stylusPointDescriptionSuperset
    /// <returns></returns>
    bool IsSubsetOf(SharedPointer<StylusPointDescription> stylusPointDescriptionSuperset) const;

    /// <summary>
    /// Returns the index of the given StylusPointProperty, or -1 if none is found
    /// </summary>
    /// <param name="propertyId">propertyId
    int IndexOf(Guid propertyId) const;

};

INKCANVAS_END_NAMESPACE

#endif // WINDOWS_INPUT_STYLUSPOINTDESCRIPTION_H
