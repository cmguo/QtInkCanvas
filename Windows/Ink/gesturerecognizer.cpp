#include "Windows/Ink/gesturerecognizer.h"
#include "Windows/Ink/strokecollection.h"
#include "Internal/Ink/GestureRecognition/nativerecognizer.h"

INKCANVAS_BEGIN_NAMESPACE

/// <summary>
/// The default constructor which enables all the application gestures.
/// </summary>
GestureRecognizer::GestureRecognizer()
    : GestureRecognizer ( { ApplicationGesture::AllGestures } )
{
}

/// <summary>
/// The constructor which take an array of the enabled application gestures.
/// </summary>
/// <param name="enabledApplicationGestures"></param>
GestureRecognizer::GestureRecognizer(List<ApplicationGesture> enabledApplicationGestures)
{
    _nativeRecognizer = NativeRecognizer::CreateInstance();
    if (_nativeRecognizer == nullptr)
    {
        //just verify the gestures
        NativeRecognizer::GetApplicationGestureArrayAndVerify(enabledApplicationGestures);
    }
    else
    {
        //
        // we only set this if _nativeRecognizer is non nullptr
        // (available) because there is no way to use this state
        // otherwise.
        //
        SetEnabledGestures(enabledApplicationGestures);
    }
}

//#endregion Constructors

//-------------------------------------------------------------------------------
//
// Methods
//
//-------------------------------------------------------------------------------

//#region Methods

/// <summary>
/// Set the enabled gestures
/// </summary>
/// <param name="applicationGestures"></param>
/// <remarks>
///     We wrap the System.Runtime.InteropServices.COMException with a more specific one
///
///  	Do wrap specific exceptions in a more descriptive exception when appropriate.
///     Int32 GetInt(Int32[] array, Int32 index){
///     try{
///         return array[index];
///     }catch(IndexOutOfRangeException e){
///         throw new ArgumentOutOfRangeException(
///             Parameter index is out of range.);
///     }
///     }
///
/// </remarks>
void GestureRecognizer::SetEnabledGestures(List<ApplicationGesture> applicationGestures)
{
    VerifyAccess();
    VerifyDisposed();
    VerifyRecognizerAvailable();
    //don't verfify the gestures, NativeRecognizer.SetEnabledGestures does it
    // since it is the TAS boundary

    //
    // we don't wrap the COM exceptions generated from the Recognizer
    // with our own exception
    //
    List<ApplicationGesture> enabledGestures =
        _nativeRecognizer->SetEnabledGestures(applicationGestures);

    //only update the state when SetEnabledGestures succeeds (since it verifies the array)
    _enabledGestures = enabledGestures;

}

/// <summary>
/// Get the enabled gestures
/// </summary>
/// <returns></returns>
List<ApplicationGesture> GestureRecognizer::GetEnabledGestures()
{
    VerifyAccess();
    VerifyDisposed();
    VerifyRecognizerAvailable();

    //can be nullptr if the call to SetEnabledGestures failed
    //if (_enabledGestures == nullptr)
    //{
    //    _enabledGestures = new ApplicationGesture[] { };
    //}
    return _enabledGestures;
}

/// <summary>
/// Performs gesture recognition on the StrokeCollection if a gesture recognizer
/// is present and installed on the system.  If not, this method throws an InvalidOperationException.
/// To determine if this method will throw an exception, only call this method if
/// the RecognizerAvailable property returns true.
///
/// </summary>
/// <param name="strokes">The StrokeCollection to perform gesture recognition on</param>
/// <returns></returns>
/// <remarks>Callers must have UnmanagedCode permission to call this API.</remarks>
/// <SecurityNote>
///     Critical: Calls SecurityCritical method RecognizeImpl
///
///     PublicOK: We demand UnmanagedCode before proceeding and
///             UnmanagedCode is the only permission we assert.
/// </SecurityNote>
//[SecurityCritical]
List<GestureRecognitionResult> GestureRecognizer::Recognize(SharedPointer<StrokeCollection> strokes)
{
    //
    // due to possible exploits in the Tablet PC Gesture recognizer's Recognize method,
    // we demand unmanaged code.
    //
    //SecurityHelper.DemandUnmanagedCode();

    return RecognizeImpl(strokes);
}

/// <summary>
/// Performs gesture recognition on the StrokeCollection if a gesture recognizer
/// is present and installed on the system.  If not, this method throws an InvalidOperationException.
/// To determine if this method will throw an exception, only call this method if
/// the RecognizerAvailable property returns true.
/// </summary>
/// <param name="strokes">The StrokeCollection to perform gesture recognition on</param>
/// <returns></returns>
/// <SecurityNote>
///     Critical: Calls a SecurityCritical method RecognizeImpl.
///
///     FriendAccess is allowed so the critical method InkCanvas.RaiseGestureOrStrokeCollected
///         can use this method
/// </SecurityNote>
// Built into Core, also used by Framework.
//[SecurityCritical]
List<GestureRecognitionResult> GestureRecognizer::CriticalRecognize(SharedPointer<StrokeCollection> strokes)
{
    return RecognizeImpl(strokes);
}


/// <summary>
/// Performs gesture recognition on the StrokeCollection if a gesture recognizer
/// is present and installed on the system.
/// </summary>
/// <param name="strokes">The StrokeCollection to perform gesture recognition on</param>
/// <returns></returns>
/// <SecurityNote>
///     Critical: Calls SecurityCritical method NativeRecognizer.Recognize
/// </SecurityNote>
//[SecurityCritical]
List<GestureRecognitionResult> GestureRecognizer::RecognizeImpl(SharedPointer<StrokeCollection> strokes)
{
    if (strokes == nullptr)
    {
        throw std::exception("strokes"); // nullptr is not allowed as the argument value
    }
    if (strokes->Count() > 2)
    {
        throw std::exception("strokes");
    }
    VerifyAccess();
    VerifyDisposed();
    VerifyRecognizerAvailable();

    return List<GestureRecognitionResult>(_nativeRecognizer->Recognize(strokes));
}



//#endregion Methods

//-------------------------------------------------------------------------------
//
// Properties
//
//-------------------------------------------------------------------------------

//#region Properties

/// <summary>
/// Indicates if a GestureRecognizer is present and installed on the system.
/// </summary>
bool GestureRecognizer::IsRecognizerAvailable()
{
    VerifyAccess();
    VerifyDisposed();

    if (_nativeRecognizer == nullptr)
    {
        return false;
    }
    return true;
}

//#endregion Properties

//-------------------------------------------------------------------------------
//
// IDisposable
//
//-------------------------------------------------------------------------------

//#region IDisposable

/// <summary>
/// Dispose this GestureRecognizer
/// </summary>
void GestureRecognizer::Dispose()
{
    VerifyAccess();

    // A simple pattern of the dispose implementation.
    // There is no finalizer since the SafeHandle in the NativeRecognizer will release the context properly.
    if ( _disposed )
    {
        return;
    }

    // NTRAID-WINDOWSBUG#1102945-WAYNEZEN,
    // Since the constructor might create a nullptr _nativeRecognizer,
    // here we have to make sure we do have some thing to dispose.
    // Otherwise just no-op.
    if ( _nativeRecognizer != nullptr )
    {
        _nativeRecognizer->Dispose();
        delete _nativeRecognizer;
        _nativeRecognizer = nullptr;
    }

    _disposed = true;
}

//#endregion IDisposable

//-------------------------------------------------------------------------------
//
// Methods
//
//-------------------------------------------------------------------------------
//#region Methods
//verify that there is a recognizer available, throw if not
void GestureRecognizer::VerifyRecognizerAvailable()
{
    if (_nativeRecognizer == nullptr)
    {
        throw std::exception("SR.Get(SRID.GestureRecognizerNotAvailable)");
    }
}

// Verify whether this object has been disposed.
void GestureRecognizer::VerifyDisposed()
{
    if ( _disposed )
    {
        throw std::exception("GestureRecognizer");
    }
}

INKCANVAS_END_NAMESPACE
