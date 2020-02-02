#ifndef STYLUSPOINTDESCRIPTION_H
#define STYLUSPOINTDESCRIPTION_H

#include "Windows/Input/styluspointpropertyinfo.h"

#include <QVector>
#include <QSharedPointer>

#include <iterator>

// namespace System.Windows.Input

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
    QVector<StylusPointPropertyInfo>   _stylusPointPropertyInfos;

    /// <summary>
    /// StylusPointDescription
    /// </summary>
    StylusPointDescription();

    /// <summary>
    /// StylusPointDescription
    /// </summary>
    StylusPointDescription(QVector<StylusPointPropertyInfo> const & stylusPointPropertyInfos);

    /// <summary>
    /// StylusPointDescription
    /// </summary>
    /// <param name="stylusPointPropertyInfos">stylusPointPropertyInfos
    /// <param name="originalPressureIndex">originalPressureIndex - does the digitizer really support pressure?  If so, the index this was at
    StylusPointDescription(QVector<StylusPointPropertyInfo> const & stylusPointPropertyInfos, int originalPressureIndex);
    /// <summary>
    /// HasProperty
    /// </summary>
    /// <param name="stylusPointProperty">stylusPointProperty
    bool HasProperty(StylusPointProperty& stylusPointProperty) const;

    /// <summary>
    /// The count of properties this StylusPointDescription contains
    /// </summary>
    int PropertyCount() const;

    /// <summary>
    /// GetProperty
    /// </summary>
    /// <param name="stylusPointProperty">stylusPointProperty
    StylusPointPropertyInfo GetPropertyInfo(StylusPointProperty & stylusPointProperty) const;

    /// <summary>
    /// GetPropertyInfo
    /// </summary>
    /// <param name="QUuid">QUuid
    StylusPointPropertyInfo GetPropertyInfo(QUuid const & QUuid) const;

    /// <summary>
    /// Returns the index of the given StylusPointProperty by ID, or -1 if none is found
    /// </summary>
    int GetPropertyIndex(QUuid QUuid) const;

    /// <summary>
    /// GetStylusPointProperties
    /// </summary>
    QVector<StylusPointPropertyInfo> GetStylusPointProperties() const;

    /// <summary>
    /// GetStylusPointPropertyIdss
    /// </summary>
    QVector<QUuid> GetStylusPointPropertyIds() const;

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
    static bool AreCompatible(QSharedPointer<StylusPointDescription> stylusPointDescription1, QSharedPointer<StylusPointDescription> stylusPointDescription2);

    /// <summary>
    /// Returns a new StylusPointDescription with the common StylusPointProperties from both
    /// </summary>
    /// <param name="stylusPointDescription">stylusPointDescription
    /// <param name="stylusPointDescriptionPreserveInfo">stylusPointDescriptionPreserveInfo
    /// <remarks>The StylusPointProperties from stylusPointDescriptionPreserveInfo will be returned in the new StylusPointDescription</remarks>
    static QSharedPointer<StylusPointDescription> GetCommonDescription(QSharedPointer<StylusPointDescription> stylusPointDescription,
                                                         QSharedPointer<StylusPointDescription> stylusPointDescriptionPreserveInfo);

    /// <summary>
    /// Returns true if this StylusPointDescription is a subset
    /// of the StylusPointDescription passed in
    /// </summary>
    /// <param name="stylusPointDescriptionSuperset">stylusPointDescriptionSuperset
    /// <returns></returns>
    bool IsSubsetOf(QSharedPointer<StylusPointDescription> stylusPointDescriptionSuperset) const;

    /// <summary>
    /// Returns the index of the given StylusPointProperty, or -1 if none is found
    /// </summary>
    /// <param name="propertyId">propertyId
    int IndexOf(QUuid propertyId) const;

};

#endif // STYLUSPOINTDESCRIPTION_H
