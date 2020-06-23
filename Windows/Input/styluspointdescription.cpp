#include "Windows/Input/styluspointdescription.h"
#include "Windows/Input/styluspointpropertyinfodefaults.h"

#include <QList>

INKCANVAS_BEGIN_NAMESPACE

StylusPointDescription::StylusPointDescription()
{
    //implement the default packet description
    _stylusPointPropertyInfos = QVector<StylusPointPropertyInfo>( {
            StylusPointPropertyInfoDefaults::X,
            StylusPointPropertyInfoDefaults::Y,
            StylusPointPropertyInfoDefaults::NormalPressure
        });
}

StylusPointDescription::StylusPointDescription(QVector<StylusPointPropertyInfo> const & stylusPointPropertyInfos)
{
    QVector<StylusPointPropertyInfo> infos(stylusPointPropertyInfos);

    if (infos.size() < RequiredCountOfProperties ||
        infos[RequiredXIndex].Id() != StylusPointPropertyIds::X ||
        infos[RequiredYIndex].Id() != StylusPointPropertyIds::Y ||
        infos[RequiredPressureIndex].Id() != StylusPointPropertyIds::NormalPressure)
    {
        throw std::runtime_error("stylusPointPropertyInfos");
    }

    //
    // look for duplicates, validate that buttons are last
    //
    QList<QUuid> seenIds;
    seenIds.push_back(StylusPointPropertyIds::X);
    seenIds.push_back(StylusPointPropertyIds::Y);
    seenIds.push_back(StylusPointPropertyIds::NormalPressure);

    int buttonCount = 0;
    for (int x = RequiredCountOfProperties; x < infos.size(); x++)
    {
        if (seenIds.contains(infos[x].Id()))
        {
            throw std::runtime_error("stylusPointPropertyInfos");
        }
        if (infos[x].IsButton())
        {
            buttonCount++;
        }
        else
        {
            //this is not a button, make sure we haven't seen one before
            if (buttonCount > 0)
            {
                throw std::runtime_error("stylusPointPropertyInfos");
            }
        }
        seenIds.push_back(infos[x].Id());
    }
    if (buttonCount > MaximumButtonCount)
    {
        throw std::runtime_error("stylusPointPropertyInfos");
    }

    _buttonCount = buttonCount;
    _stylusPointPropertyInfos = infos;
}

StylusPointDescription::StylusPointDescription(QVector<StylusPointPropertyInfo> const & stylusPointPropertyInfos, int originalPressureIndex)
    : StylusPointDescription (stylusPointPropertyInfos)
{
    _originalPressureIndex = originalPressureIndex;
}

/// <summary>
/// HasProperty
/// </summary>
/// <param name="stylusPointProperty">stylusPointProperty
bool StylusPointDescription::HasProperty(StylusPointProperty & stylusPointProperty) const
{
    int index = IndexOf(stylusPointProperty.Id());
    if (-1 == index)
    {
        return false;
    }
    return true;
}

/// <summary>
/// The count of properties this StylusPointDescription contains
/// </summary>
int StylusPointDescription::PropertyCount() const
{
    return _stylusPointPropertyInfos.size();
}

/// <summary>
/// GetProperty
/// </summary>
/// <param name="stylusPointProperty">stylusPointProperty
StylusPointPropertyInfo StylusPointDescription::GetPropertyInfo(StylusPointProperty& stylusPointProperty) const
{
    return GetPropertyInfo(stylusPointProperty.Id());
}

/// <summary>
/// GetPropertyInfo
/// </summary>
/// <param name="QUuid">QUuid
StylusPointPropertyInfo StylusPointDescription::GetPropertyInfo(QUuid const & QUuid) const
{
    int index = IndexOf(QUuid);
    if (-1 == index)
    {
        //we didn't find it
        throw std::runtime_error("stylusPointProperty");
    }
    return _stylusPointPropertyInfos[index];
}

/// <summary>
/// Returns the index of the given StylusPointProperty by ID, or -1 if none is found
/// </summary>
int StylusPointDescription::GetPropertyIndex(QUuid QUuid) const
{
    return IndexOf(QUuid);
}

/// <summary>
/// GetStylusPointProperties
/// </summary>
QVector<StylusPointPropertyInfo> StylusPointDescription::GetStylusPointProperties() const
{
    return _stylusPointPropertyInfos;
}

/// <summary>
/// GetStylusPointPropertyIdss
/// </summary>
QVector<QUuid> StylusPointDescription::GetStylusPointPropertyIds() const
{
    QVector<QUuid> ret(_stylusPointPropertyInfos.size());
    for (int x = 0; x < ret.size(); x++)
    {
        ret[x] = _stylusPointPropertyInfos[x].Id();
    }
    return ret;
}

/// <summary>
/// helper for determining how many ints in a raw int array
/// correspond to one point we get from the input system
/// </summary>
int StylusPointDescription::GetInputArrayLengthPerPoint() const
{
    int buttonLength = _buttonCount > 0 ? 1 : 0;
    int propertyLength = (_stylusPointPropertyInfos.size() - _buttonCount) + buttonLength;
    if (!ContainsTruePressure())
    {
        propertyLength--;
    }
    return propertyLength;
}

/// <summary>
/// helper for determining how many members a StylusPoint's
/// int[] should be for additional data
/// </summary>
int StylusPointDescription::GetExpectedAdditionalDataCount() const
{
    int buttonLength = _buttonCount > 0 ? 1 : 0;
    int expectedLength = ((_stylusPointPropertyInfos.size() - _buttonCount) + buttonLength) - 3 /*x, y, p*/;
    return expectedLength;
}

/// <summary>
/// helper for determining how many ints in a raw int array
/// correspond to one point when saving to himetric
/// </summary>
/// <returns></returns>
int StylusPointDescription::GetOutputArrayLengthPerPoint() const
{
    int length = GetInputArrayLengthPerPoint();
    if (!ContainsTruePressure())
    {
        length++;
    }
    return length;
}

/// <summary>
/// helper for determining how many buttons are present
/// </summary>
int StylusPointDescription::ButtonCount() const
{
    return _buttonCount;
}

/// <summary>
/// helper for determining what bit position the button is at
/// </summary>
int StylusPointDescription::GetButtonBitPosition(StylusPointProperty buttonProperty) const
{
    if (!buttonProperty.IsButton())
    {
        throw std::runtime_error("");
    }
    int buttonIndex = 0;
    for (int x = _stylusPointPropertyInfos.size() - _buttonCount; //start of the buttons
         x < _stylusPointPropertyInfos.size(); x++)
    {
        if (_stylusPointPropertyInfos[x].Id() == buttonProperty.Id())
        {
            return buttonIndex;
        }
        if (_stylusPointPropertyInfos[x].IsButton())
        {
            // we're in the buttons, but this isn't the right one,
            // bump the button index and keep looking
            buttonIndex++;
        }
    }
    return -1;
}

/// <summary>
/// ContainsTruePressure - true if this StylusPointDescription was instanced
/// by a TabletDevice or by ISF serialization that contains NormalPressure
/// </summary>
bool StylusPointDescription::ContainsTruePressure() const
{
    return (_originalPressureIndex != -1);
}

/// <summary>
/// helper to determine the original pressure index
/// </summary>
int StylusPointDescription::OriginalPressureIndex() const
{
    return _originalPressureIndex;
}

/// <summary>
/// Returns true if the two StylusPointDescriptions have the same StylusPointProperties.  Metrics are ignored.
/// </summary>
/// <param name="stylusPointDescription1">stylusPointDescription1
/// <param name="stylusPointDescription2">stylusPointDescription2
bool StylusPointDescription::AreCompatible(QSharedPointer<StylusPointDescription> stylusPointDescription1,
                                           QSharedPointer<StylusPointDescription> stylusPointDescription2)
{
    //
    // ignore X, Y, Pressure - they are guaranteed to be the first3 members
    //
    //Debug.Assert(   stylusPointDescription1._stylusPointPropertyInfos.Length >= RequiredCountOfProperties &&
    //                stylusPointDescription1._stylusPointPropertyInfos[0].Id == StylusPointPropertyIds.X &&
    //                stylusPointDescription1._stylusPointPropertyInfos[1].Id == StylusPointPropertyIds.Y &&
    //                stylusPointDescription1._stylusPointPropertyInfos[2].Id == StylusPointPropertyIds.NormalPressure);

    //Debug.Assert(   stylusPointDescription2._stylusPointPropertyInfos.Length >= RequiredCountOfProperties &&
    //                stylusPointDescription2._stylusPointPropertyInfos[0].Id == StylusPointPropertyIds.X &&
    //                stylusPointDescription2._stylusPointPropertyInfos[1].Id == StylusPointPropertyIds.Y &&
    //                stylusPointDescription2._stylusPointPropertyInfos[2].Id == StylusPointPropertyIds.NormalPressure);

    if (stylusPointDescription1->_stylusPointPropertyInfos.size() != stylusPointDescription2->_stylusPointPropertyInfos.size())
    {
        return false;
    }
    for (int x = RequiredCountOfProperties; x < stylusPointDescription1->_stylusPointPropertyInfos.size(); x++)
    {
        if (!StylusPointPropertyInfo::AreCompatible(stylusPointDescription1->_stylusPointPropertyInfos[x], stylusPointDescription2->_stylusPointPropertyInfos[x]))
        {
            return false;
        }
    }

    return true;
}

/// <summary>
/// Returns a new StylusPointDescription with the common StylusPointProperties from both
/// </summary>
/// <param name="stylusPointDescription">stylusPointDescription
/// <param name="stylusPointDescriptionPreserveInfo">stylusPointDescriptionPreserveInfo
/// <remarks>The StylusPointProperties from stylusPointDescriptionPreserveInfo will be returned in the new StylusPointDescription</remarks>
QSharedPointer<StylusPointDescription> StylusPointDescription::GetCommonDescription(QSharedPointer<StylusPointDescription> stylusPointDescription,
                                                                      QSharedPointer<StylusPointDescription> stylusPointDescriptionPreserveInfo)
{
    //
    // ignore X, Y, Pressure - they are guaranteed to be the first3 members
    //
    //Debug.Assert(stylusPointDescription._stylusPointPropertyInfos.Length >= 3 &&
    //                stylusPointDescription._stylusPointPropertyInfos[0].Id == StylusPointPropertyIds.X &&
    //                stylusPointDescription._stylusPointPropertyInfos[1].Id == StylusPointPropertyIds.Y &&
    //                stylusPointDescription._stylusPointPropertyInfos[2].Id == StylusPointPropertyIds.NormalPressure);

    //Debug.Assert(stylusPointDescriptionPreserveInfo._stylusPointPropertyInfos.Length >= 3 &&
    //                stylusPointDescriptionPreserveInfo._stylusPointPropertyInfos[0].Id == StylusPointPropertyIds.X &&
    //                stylusPointDescriptionPreserveInfo._stylusPointPropertyInfos[1].Id == StylusPointPropertyIds.Y &&
    //                stylusPointDescriptionPreserveInfo._stylusPointPropertyInfos[2].Id == StylusPointPropertyIds.NormalPressure);


    //add x, y, p
    QVector<StylusPointPropertyInfo> commonProperties;
    commonProperties.push_back(stylusPointDescriptionPreserveInfo->_stylusPointPropertyInfos[0]);
    commonProperties.push_back(stylusPointDescriptionPreserveInfo->_stylusPointPropertyInfos[1]);
    commonProperties.push_back(stylusPointDescriptionPreserveInfo->_stylusPointPropertyInfos[2]);

    //add common properties
    for (int x = RequiredCountOfProperties; x < stylusPointDescription->_stylusPointPropertyInfos.size(); x++)
    {
        for (int y = RequiredCountOfProperties; y < stylusPointDescriptionPreserveInfo->_stylusPointPropertyInfos.size(); y++)
        {
            if (StylusPointPropertyInfo::AreCompatible(  stylusPointDescription->_stylusPointPropertyInfos[x],
                                                    stylusPointDescriptionPreserveInfo->_stylusPointPropertyInfos[y]))
            {
                commonProperties.push_back(stylusPointDescriptionPreserveInfo->_stylusPointPropertyInfos[y]);
            }
        }
    }

    return QSharedPointer<StylusPointDescription>(new StylusPointDescription(commonProperties));
}

/// <summary>
/// Returns true if this StylusPointDescription is a subset
/// of the StylusPointDescription passed in
/// </summary>
/// <param name="stylusPointDescriptionSuperset">stylusPointDescriptionSuperset
/// <returns></returns>
bool StylusPointDescription::IsSubsetOf(QSharedPointer<StylusPointDescription> stylusPointDescriptionSuperset) const
{
    if (nullptr == stylusPointDescriptionSuperset)
    {
        throw std::runtime_error("stylusPointDescriptionSuperset");
    }
    if (stylusPointDescriptionSuperset->_stylusPointPropertyInfos.size() < _stylusPointPropertyInfos.size())
    {
        return false;
    }
    //
    // iterate through our local properties and make sure that the
    // superset contains them
    //
    for (int x = 0; x < _stylusPointPropertyInfos.size(); x++)
    {
        QUuid id = _stylusPointPropertyInfos[x].Id();
        if (-1 == stylusPointDescriptionSuperset->IndexOf(id))
        {
            return false;
        }
    }
    return true;
}

/// <summary>
/// Returns the index of the given StylusPointProperty, or -1 if none is found
/// </summary>
/// <param name="propertyId">propertyId
int StylusPointDescription::IndexOf(QUuid propertyId) const
{
    for (int x = 0; x < _stylusPointPropertyInfos.size(); x++)
    {
        if (_stylusPointPropertyInfos[x].Id() == propertyId)
        {
            return x;
        }
    }
    return -1;
}

INKCANVAS_END_NAMESPACE
