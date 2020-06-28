#ifndef NATIVERECOGNIZER_H
#define NATIVERECOGNIZER_H

#include "InkCanvas_global.h"
#include "Windows/Ink/applicationgesture.h"
#include "Windows/Ink/gesturerecognitionresult.h"
#include "Collections/Generic/list.h"
#include "guid.h"
#include "sharedptr.h"

#include <QMutex>

#include <Windows.h>
extern "C"
{
#include <recapis.h>
}

INKCANVAS_BEGIN_NAMESPACE

class Stroke;
class StrokeCollection;

// namespace MS.Internal.Ink.GestureRecognition

/// <summary>
/// NativeRecognizer class
/// </summary>
class NativeRecognizer
{
    //-------------------------------------------------------------------------------
    //
    // Constructors
    //
    //-------------------------------------------------------------------------------

    //#region Constructors

public:
    /// <summary>
    /// Static constructor
    /// </summary>
    /// <SecurityNote>
    ///     Critical: Calls a SecurityCritical methods
    ///                 LoadRecognizerDll();
    /// </SecurityNote>
    //[SecurityCritical]
    static void Init();

private:
    /// <summary>
    /// constructor
    /// </summary>
    /// <SecurityNote>
    ///     Critical: Calls a SecurityCritical method
    ///                 NativeRecognizer.UnsafeNativeMethods.CreateContext()
    ///               Accesses the SecurityCritical member
    ///                 _hContext
    /// </SecurityNote>
    //[SecurityCritical]
    NativeRecognizer();

    //#endregion Constructors

    //-------------------------------------------------------------------------------
    //
    // Methods
    //
    //-------------------------------------------------------------------------------

    //#region Methods

public:
    /// <summary>
    /// Create an Instance of the NativeRecognizer.
    /// </summary>
    /// <returns>null if it fails</returns>
    /// <SecurityNote>
    ///     Critical: Calls a SecurityCritical method
    ///                 NativeRecognizer();
    ///     TreatAsSafe: The method is safe because no arguments are passed.
    ///         The NativeRecognizer return value is protected with SecurityCritical
    ///         attributes
    /// </SecurityNote>
    //[SecurityCritical, SecurityTreatAsSafe]
    static NativeRecognizer* CreateInstance();

    /// <summary>
    /// Set the enabled gestures
    /// </summary>
    /// <param name="applicationGestures"></param>
    /// <SecurityNote>
    ///     Critical: Handles _hContext, which is SecurityCritical
    ///     TreatAsSafe: The method is safe because argument passed can not be
    ///         used maliciously. And we verify the length of the passed in array.
    /// </SecurityNote>
    //[SecurityCritical, SecurityTreatAsSafe]
    List<ApplicationGesture> SetEnabledGestures(List<ApplicationGesture> applicationGestures);

    /// <summary>
    /// Recognize the strokes.
    /// </summary>
    /// <param name="strokes"></param>
    /// <returns></returns>
    /// <SecurityNote>
    ///     Critical: Calls a SecurityCritical method
    ///             NativeRecognizer.UnsafeNativeMethods.ResetContext,
    ///             AddStrokes,
    ///             NativeRecognizer.UnsafeNativeMethods.Process
    ///             InvokeGetAlternateList
    ///             InvokeGetLatticePtr
    /// </SecurityNote>
    //[SecurityCritical]
    List<GestureRecognitionResult> Recognize(SharedPointer<StrokeCollection> strokes);


    static List<ApplicationGesture> GetApplicationGestureArrayAndVerify(List<ApplicationGesture> applicationGestures);

    //#endregion Methods

    //-------------------------------------------------------------------------------
    //
    // IDisposable
    //
    //-------------------------------------------------------------------------------

    //#region IDisposable

    /// <summary>
    /// A simple pattern of the dispose implementation.
    /// There is no finalizer since the SafeHandle will take care of releasing the context.
    /// </summary>
    /// <SecurityNote>
    ///     Critical: Calls a SecurityCritical method and the SecurityCritical handle
    ///                 _hContext.Dispose()
    ///     TreatAsSafe: The method is safe because no arguments are passed.  We guard
    ///                 against dispose being called twice.
    /// </SecurityNote>
    //[SecurityCritical, SecurityTreatAsSafe]
    void Dispose();

    //#endregion IDisposable

    //-------------------------------------------------------------------------------
    //
    // Methods
    //
    //-------------------------------------------------------------------------------

    //#region Methods
private:
    /// <summary>
    ///  Load the mshwgst.dll from the path in the registry. Make sure this loading action prior to invoking
    ///  any native functions marked with DllImport in mshwgst.dll
    ///  This method is called from the NativeRecognizer's static construtor.
    /// </summary>
    /// <SecurityNote>
    ///     Critical: Requires read registry and unmanaged code access
    /// </SecurityNote>
    //[SecurityCritical]
    static bool LoadRecognizerDll();

    /// <summary>
    /// Set the enabled gestures.
    /// This method is called from the SetEnabledGestures method.
    /// </summary>
    /// <SecurityNote>
    ///     Critical: Calls a critical pinvoke
    ///                 NativeRecognizer.UnsafeNativeMethods.SetEnabledUnicodeRanges
    /// </SecurityNote>
    //[SecurityCritical]
    int SetEnabledGestures(HRECOCONTEXT recContext, List<ApplicationGesture> enabledGestures);

    /// <summary>
    /// Add the strokes to the recoContext.
    /// The method is called from the Recognize method.
    /// </summary>
    /// <SecurityNote>
    ///     Critical: Calls a critical PInvoke
    ///                 GetPacketData,
    ///                 NativeRecognizer.UnsafeNativeMethods.AddStroke,
    ///                 ReleaseResourcesinPacketDescription
    /// </SecurityNote>
    //[SecurityCritical]
    int AddStrokes(HRECOCONTEXT recContext, SharedPointer<StrokeCollection> strokes);

    /// <summary>
    /// Retrieve the packet description, packets data and XFORM which is the information the native recognizer needs.
    /// The method is called from AddStrokes.
    /// </summary>
    /// <SecurityNote>
    ///     Critical: Contains unsafe code
    /// </SecurityNote>
    //[SecurityCritical]
    void GetPacketData
    (
        SharedPointer<Stroke> stroke,
        PACKET_DESCRIPTION& packetDescription,
        int& countOfBytes,
        QVector<int>& packets,
        XFORM& xForm
    );

    /// <summary>
    /// Release the memory blocks which has been created for mashalling purpose.
    /// The method is called from AddStrokes.
    /// </summary>
    /// <SecurityNote>
    ///     Critical: Calls unsafe code, requires UnmanageCode permission
    /// </SecurityNote>
    //[SecurityCritical]
    void ReleaseResourcesinPacketDescription(PACKET_DESCRIPTION& pd, QVector<int>& packets);

    /// <summary>
    /// Invokes GetAlternateList in the native dll
    /// </summary>
    /// <returns></returns>
    /// <SecurityNote>
    ///     Critical: Calls the native methods
    ///                 NativeRecognizer.UnsafeNativeMethods.GetAlternateList
    ///                 NativeRecognizer.UnsafeNativeMethods.GetString
    ///                 NativeRecognizer.UnsafeNativeMethods.GetConfidenceLevel
    ///                 NativeRecognizer.UnsafeNativeMethods.DestroyAlternate
    /// </SecurityNote>
    //[SecurityCritical]
    List<GestureRecognitionResult> InvokeGetAlternateList();

    /// <summary>
    /// Invokes GetLatticePtr in the native dll
    /// </summary>
    /// <returns></returns>
    /// <SecurityNote>
    ///     Critical: Calls the native methods
    ///                 NativeRecognizer.UnsafeNativeMethods.GetLatticePtr
    ///               And uses unsafe code
    /// </SecurityNote>
    //[SecurityCritical]
    List<GestureRecognitionResult> InvokeGetLatticePtr();

    //#endregion Methods


    /// <summary>
    /// RecognizerHandle is a static property. But it's a SafeHandle.
    /// So, we don't have to worry about releasing the handle since RecognizerSafeHandle when there is no reference on it.
    /// </summary>
    /// <SecurityNote>
    ///     Critical: Calls a SecurityCritical pinvoke and accesses SecurityCritical fields
    /// </SecurityNote>
    static HRECOGNIZER RecognizerHandleSingleton();


    enum RECO_TYPE
    {
        RECO_TYPE_WSTRING = 0,
        RECO_TYPE_WCHAR = 1
    };

    static constexpr char const * GestureRecognizerPath = "SOFTWARE\\MICROSOFT\\TPG\\SYSTEM RECOGNIZERS\\{BED9A940-7D48-48E3-9A68-F4887A5A1B2E}";
    static constexpr char const * GestureRecognizerFullPath = "HKEY_LOCAL_MACHINE\\SOFTWARE\\MICROSOFT\\TPG\\SYSTEM RECOGNIZERS\\{BED9A940-7D48-48E3-9A68-F4887A5A1B2E}";
    static constexpr char const * GestureRecognizerValueName = "RECOGNIZER DLL";
    static constexpr char const * GestureRecognizerGuid = "{BED9A940-7D48-48E3-9A68-F4887A5A1B2E}";

    typedef unsigned short ushort;

    // This constant is an identical value as the one in drivers\tablet\recognition\ink\core\common\inc\gesture\gesturedefs.h
    static constexpr ushort MAX_GESTURE_COUNT = 256;
    // This constant is an identical value as the one in drivers\tablet\include\sdk\recdefs.h
    //static constexpr ushort GESTURE_NULL = 0xf000;
    // This constant is an identical value as the one in public\internal\drivers\inc\msinkaut.h
    static constexpr ushort IRAS_DefaultCount = 10;
    static constexpr ushort MaxStylusPoints = 10000;

    // The Guid has been copied from public\internal\drivers\inc\tpcGuid.h
    //// {7DFE11A7-FB5D-4958-8765-154ADF0D833F}
    //DEFINE_Guid(GUID_CONFIDENCELEVEL, 0x7dfe11a7, 0xfb5d, 0x4958, 0x87, 0x65, 0x15, 0x4a, 0xdf, 0xd, 0x83, 0x3f);
    static const Guid GUID_CONFIDENCELEVEL;

    /// <summary>
    /// IDisposable support
    /// </summary>
    bool                                    _disposed = false;

    /// <summary>
    /// Each NativeRecognizer instance has it's own recognizer context
    /// </summary>
    /// <SecurityNote>
    ///     Critical: The SecurityCritical handle
    /// </SecurityNote>
    //[SecurityCritical]
    HRECOCONTEXT _hContext;

    /// <summary>
    /// Used to lock for instancing the native recognizer handle
    /// </summary>
    static QMutex                           _syncRoot;

    /// <summary>
    /// All NativeRecognizer share a single handle to the recognizer
    /// </summary>
    /// <SecurityNote>
    ///     Critical: The SecurityCritical handle
    /// </SecurityNote>
    //[SecurityCritical]
    static HRECOGNIZER s_hRec;

    /// <summary>
    /// The Guid of the GestureRecognizer used for registry lookup
    /// </summary>
    static Guid                             s_Gesture;

    /// <summary>
    /// can we load the recognizer?
    /// </summary>
    static bool s_isSupported;

    /// <summary>
    /// A flag indicates whether we can find the entry point of
    /// GetAlternateList function in mshwgst.dll
    /// </summary>
    static bool s_GetAlternateListExists;

    static HRESULT (WINAPI *CreateRecognizer)(CLSID *pCLSID, HRECOGNIZER *phrec);
    static HRESULT (WINAPI *DestroyRecognizer)(HRECOGNIZER hrec);

    static HRESULT (WINAPI *CreateContext)(HRECOGNIZER hrec, HRECOCONTEXT *phrc);
    static HRESULT (WINAPI *DestroyContext)(HRECOCONTEXT hrc);
    static HRESULT (WINAPI *AddStroke)(HRECOCONTEXT hrc, const PACKET_DESCRIPTION* pPacketDesc, ULONG cbPacket, const BYTE *pPacket, const XFORM *pXForm);
    static HRESULT (WINAPI *ResetContext)(HRECOCONTEXT hrc);
    static HRESULT (WINAPI *Process)(HRECOCONTEXT hrc, BOOL *pbPartialProcessing);
    static HRESULT (WINAPI *SetEnabledUnicodeRanges)(HRECOCONTEXT hrc, ULONG cRanges, CHARACTER_RANGE *pcr);
    static HRESULT (WINAPI *EndInkInput)(HRECOCONTEXT hrc);
    static HRESULT (WINAPI *GetLatticePtr)(HRECOCONTEXT hrc, RECO_LATTICE **ppLattice);

    static HRESULT (WINAPI *GetAlternateList)(HRECOCONTEXT hrc, RECO_RANGE* pRecoRange, ULONG*pcAlt, HRECOALT*phrcalt, ALT_BREAKS iBreak);
    static HRESULT (WINAPI *GetString)(HRECOALT hrcalt, RECO_RANGE *pRecoRange, ULONG* pcSize, WCHAR* pwcString);
    static HRESULT (WINAPI *GetConfidenceLevel)(HRECOALT hrcalt, RECO_RANGE* pRecoRange, CONFIDENCE_LEVEL* pcl);
    static HRESULT (WINAPI *DestroyAlternate)(HRECOALT hrcalt);
};

INKCANVAS_END_NAMESPACE

#endif // NATIVERECOGNIZER_H
