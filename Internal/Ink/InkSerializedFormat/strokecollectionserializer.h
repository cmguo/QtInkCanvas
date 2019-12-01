#ifndef STROKECOLLECTIONSERIALIZER_H
#define STROKECOLLECTIONSERIALIZER_H

// namespace MS.Internal.Ink.InkSerializedFormat

class StrokeCollectionSerializer
{
public:
    static constexpr double AvalonToHimetricMultiplier = 2540.0 / 96.0;

    static constexpr double HimetricToAvalonMultiplier = 96.0 / 2540.0;

    StrokeCollectionSerializer();
};

#endif // STROKECOLLECTIONSERIALIZER_H
