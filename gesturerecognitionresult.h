#ifndef GESTURERECOGNITIONRESULT_H
#define GESTURERECOGNITIONRESULT_H

#include "applicationgesture.h"

/// <summary>
/// RecognitionConfidence
/// </summary>
enum class RecognitionConfidence
{
    /// <summary>
    /// Strong
    /// </summary>
    Strong = 0,
    /// <summary>
    /// Intermediate
    /// </summary>
    Intermediate = 1,
    /// <summary>
    /// Poor
    /// </summary>
    Poor = 2,
};


/// <summary>
/// GestureRecognitionResult
/// </summary>
class GestureRecognitionResult
{
    //-------------------------------------------------------------------------------
    //
    // Constructors
    //
    //-------------------------------------------------------------------------------
public:
    //#region Constructors

    GestureRecognitionResult(RecognitionConfidence confidence, ApplicationGesture gesture)
    {
        _confidence = confidence;
        _gesture = gesture;
    }

    //#endregion Constructors

    //-------------------------------------------------------------------------------
    //
    // Public Properties
    //
    //-------------------------------------------------------------------------------

    //#region Public Properties

    /// <summary>
    /// RecognitionConfidence Proeprty
    /// </summary>
    RecognitionConfidence GetRecognitionConfidence()
    {
        return _confidence;
    }

    /// <summary>
    /// ApplicationGesture Property
    /// </summary>
    ApplicationGesture GetApplicationGesture()
    {
        return _gesture;
    }

    //#endregion Public Properties

    //-------------------------------------------------------------------------------
    //
    // Private Fields
    //
    //-------------------------------------------------------------------------------

    //#region Private Fields

private:
    RecognitionConfidence   _confidence;
    ApplicationGesture      _gesture;

    //#endregion Private Fields
};

#endif // GESTURERECOGNITIONRESULT_H
