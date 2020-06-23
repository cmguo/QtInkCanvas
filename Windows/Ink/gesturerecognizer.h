#ifndef GESTURERECOGNIZER_H
#define GESTURERECOGNIZER_H

#include "Windows/dependencyobject.h"
#include "applicationgesture.h"
#include "gesturerecognitionresult.h"

#include <QList>
#include <QSharedPointer>

INKCANVAS_BEGIN_NAMESPACE

class StrokeCollection;
class NativeRecognizer;

// namespace System.Windows.Ink

/// <summary>
/// Performs gesture recognition on a StrokeCollection
/// </summary>
/// <remarks>
///     No finalizer is defined because all unmanaged resources are wrapped in SafeHandle's
///     NOTE: this class provides the APIs that call into unmanaged code to perform
///         recognition.  There are two inputs that are accepted:
///         ApplicationGesture[] and StrokeCollection.
///         This class verifies the ApplicationGesture[] and StrokeCollection / Stroke are validated.
/// </remarks>
class GestureRecognizer : public DependencyObject
{
    //-------------------------------------------------------------------------------
    //
    // Constructors
    //
    //-------------------------------------------------------------------------------

    //#region Constructors
public:
    /// <summary>
    /// The default constructor which enables all the application gestures.
    /// </summary>
    GestureRecognizer();

    /// <summary>
    /// The constructor which take an array of the enabled application gestures.
    /// </summary>
    /// <param name="enabledApplicationGestures"></param>
    GestureRecognizer(QList<ApplicationGesture> enabledApplicationGestures);

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
    void SetEnabledGestures(QList<ApplicationGesture> applicationGestures);

    /// <summary>
    /// Get the enabled gestures
    /// </summary>
    /// <returns></returns>
    QList<ApplicationGesture> GetEnabledGestures();

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
    QList<GestureRecognitionResult> Recognize(QSharedPointer<StrokeCollection> strokes);

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
    QList<GestureRecognitionResult> CriticalRecognize(QSharedPointer<StrokeCollection> strokes);

private:
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
    QList<GestureRecognitionResult> RecognizeImpl(QSharedPointer<StrokeCollection> strokes);



    //#endregion Methods

    //-------------------------------------------------------------------------------
    //
    // Properties
    //
    //-------------------------------------------------------------------------------

    //#region Properties

public:
    /// <summary>
    /// Indicates if a GestureRecognizer is present and installed on the system.
    /// </summary>
    bool IsRecognizerAvailable();

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
    void Dispose();

    //#endregion IDisposable

    //-------------------------------------------------------------------------------
    //
    // Methods
    //
    //-------------------------------------------------------------------------------
private:
    //#region Methods
    //verify that there is a recognizer available, throw if not
    void VerifyRecognizerAvailable();

    // Verify whether this object has been disposed.
    void VerifyDisposed();

    //#endregion Methods

    //-------------------------------------------------------------------------------
    //
    // Fields
    //
    //-------------------------------------------------------------------------------

    //#region Fields

    QList<ApplicationGesture>        _enabledGestures;
    NativeRecognizer*            _nativeRecognizer;
    bool                        _disposed;

    //#endregion Fields

};

INKCANVAS_END_NAMESPACE

#endif // GESTURERECOGNIZER_H
