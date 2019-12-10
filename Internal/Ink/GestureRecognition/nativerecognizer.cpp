#include "Internal/Ink/GestureRecognition/nativerecognizer.h"
#include "Windows/Ink/strokecollection.h"
#include "Windows/Input/styluspointdescription.h"
#include "Windows/Input/styluspointproperties.h"
#include "Windows/Input/styluspointpropertyinfodefaults.h"
#include "Windows/Ink/stroke.h"
#include "Internal/finallyhelper.h"
#include "Internal/debug.h"

/// <summary>
/// constructor
/// </summary>
/// <SecurityNote>
///     Critical: Calls a SecurityCritical methods
///                 LoadRecognizerDll();
/// </SecurityNote>
//[SecurityCritical]
void NativeRecognizer::Init()
{
    s_isSupported = LoadRecognizerDll();
}

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
NativeRecognizer::NativeRecognizer()
{
    Debug::Assert(NativeRecognizer::RecognizerHandleSingleton() != nullptr);

    int hr = ::CreateContext(NativeRecognizer::RecognizerHandleSingleton(),
                                                                &_hContext);
    if (FAILED(hr))
    {
        //don't throw a com exception here, we don't need to pass out any details
        throw std::exception("SR.Get(SRID.UnspecifiedGestureConstructionException)");
    }

    // We add a reference of the recognizer to the context handle.
    // The context will dereference the recognizer reference when it gets disposed.
    // This trick will prevent the GC from disposing the recognizer before all contexts.
    //_hContext.AddReferenceOnRecognizer(NativeRecognizer::RecognizerHandleSingleton());
}

//#endregion Constructors

//-------------------------------------------------------------------------------
//
// Methods
//
//-------------------------------------------------------------------------------

//#region Methods

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
NativeRecognizer* NativeRecognizer::CreateInstance()
{
    if (NativeRecognizer::RecognizerHandleSingleton() != nullptr)
    {
        return new NativeRecognizer();
    }
    else
    {
        return nullptr;
    }
}

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
QList<ApplicationGesture> NativeRecognizer::SetEnabledGestures(QList<ApplicationGesture> applicationGestures)
{
    if (_disposed)
    {
        throw std::exception("NativeRecognizer");
    }

    //validate and get an array out
    QList<ApplicationGesture> enabledGestures =
        GetApplicationGestureArrayAndVerify(applicationGestures);

    // Set enabled Gestures.
    int hr = SetEnabledGestures(_hContext, enabledGestures);
    if (FAILED(hr))
    {
        //don't throw a com exception here, we don't need to pass out any details
        throw std::exception("SR.Get(SRID.UnspecifiedSetEnabledGesturesException)");
    }

    return enabledGestures;
}

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
QList<GestureRecognitionResult> NativeRecognizer::Recognize(QSharedPointer<StrokeCollection> strokes)
{
    if (_disposed)
    {
        throw std::exception("NativeRecognizer");
    }

    //
    // note that we validate this argument from GestureRecognizer
    // but since this is marked TAS, we want to do it here as well
    //
    if (strokes == nullptr)
    {
        throw std::exception("strokes"); // nullptr is not allowed as the argument value
    }
    if (strokes->size() > 2)
    {
        throw std::exception("strokes");
    }

    // Create an empty result.
    QList<GestureRecognitionResult> recResults;

    if ( strokes->size() == 0 )
    {
        return recResults;
    }

    int hr = 0;

    FinallyHelper final([&hr](){
        if ( FAILED(hr) )
        {
            //don't throw a com exception here, we don't need to pass out any details
            throw std::exception("SR.Get(SRID.UnspecifiedGestureException)");
        }
    });
    //try
    {
        // Reset the context
        hr = ::ResetContext(_hContext);
        if (FAILED(hr))
        {
            //finally block will clean up and throw
            return recResults;
        }

        // Add strokes
        hr = AddStrokes(_hContext, strokes);
        if (FAILED(hr))
        {
            //AddStrokes's finally block will clean up this finally block will throw
            return recResults;
        }

        // recognize the ink
        BOOL bIncremental;
        hr = ::Process(_hContext, &bIncremental);

        if (SUCCEEDED(hr))
        {
            if ( s_GetAlternateListExists )
            {
                recResults = InvokeGetAlternateList();
            }
            else
            {
                recResults = InvokeGetLatticePtr();
            }
        }
    }
    //finally
    //{
        // Check if we should report any error.
    //    if ( FAILED(hr) )
    //    {
            //don't throw a com exception here, we don't need to pass out any details
    //        throw std::exception(SR.Get(SRID.UnspecifiedGestureException));
    //    }
    //}

    return recResults;
}


QList<ApplicationGesture> NativeRecognizer::GetApplicationGestureArrayAndVerify(QList<ApplicationGesture> applicationGestures)
{
    //if (applicationGestures == nullptr)
    //{
        // nullptr is not allowed as the argument value
    //    throw std::exception("applicationGestures");
    //}

    int count = applicationGestures.size();
    //we need to make a disconnected copy
    //ICollection<ApplicationGesture> collection = applicationGestures as ICollection<ApplicationGesture>;
    //if (collection != nullptr)
    //{
    //    count = (uint)collection.Count;
    //}
    //else
    //{
    //    for (ApplicationGesture gesture : applicationGestures)
    //    {
    //        count++;
    //    }
    //}

    // Cannot be empty
    if (count == 0)
    {
        // An empty array is not allowed.
        throw std::exception("applicationGestures");
    }

    bool foundAllGestures = false;
    QList<ApplicationGesture> gestures;
    for (ApplicationGesture gesture : applicationGestures)
    {
        if (!ApplicationGestureHelper::IsDefined(gesture))
        {
            throw std::exception("applicationGestures");
        }

        //check for allgestures
        if (gesture == ApplicationGesture::AllGestures)
        {
            foundAllGestures = true;
        }

        //check for dupes
        if (gestures.contains(gesture))
        {
            throw std::exception("applicationGestures");
        }

        gestures.append(gesture);
    }

    // AllGesture cannot be specified with other gestures
    if (foundAllGestures && gestures.size() != 1)
    {
        // no dupes allowed
        throw std::exception("applicationGestures");
    }

    return gestures;
}

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
void NativeRecognizer::Dispose()
{
    if (_disposed)
    {
        return;
    }

    ::DestroyContext(_hContext);
    _disposed = true;
}

//#endregion IDisposable

//-------------------------------------------------------------------------------
//
// Methods
//
//-------------------------------------------------------------------------------

template <typename T>
static T GetProcAddressT(HMODULE hModule, LPCSTR lpProcName, T result)
{
    (void) result;
    return reinterpret_cast<T>(::GetProcAddress(hModule, lpProcName));
}

//#region Methods
/// <summary>
///  Load the mshwgst.dll from the path in the registry. Make sure this loading action prior to invoking
///  any native functions marked with DllImport in mshwgst.dll
///  This method is called from the NativeRecognizer's construtor.
/// </summary>
/// <SecurityNote>
///     Critical: Requires read registry and unmanaged code access
/// </SecurityNote>
//[SecurityCritical]
bool NativeRecognizer::LoadRecognizerDll()
{
    // ISSUE-2005/01/14-WAYNEZEN,
    // We may hit the problem when an application already load mshwgst.dll from somewhere rather than the
    // directory we are looking for. The options to resolve this -
    //  1. We fail the recognition functionality.
    //  2. We unload the previous mshwgst.dll
    //  3. We switch the DllImport usage to the new dynamic PInvoke mechanism in Whidbey. Please refer to the blog
    //     http://blogs.msdn.com/junfeng/archive/2004/07/14/181932.aspx. Then we don't have to unload the existing
    //     mshwgst.dll.
    //QString path = nullptr;
    char path[256] = {0};
    //System.Security.PermissionSet permissionSet = new PermissionSet(null);
    //permissionSet.AddPermission(new RegistryPermission(RegistryPermissionAccess.Read,
    //                                            System.Security.AccessControl.AccessControlActions.View,
    //                                            GestureRecognizerFullPath));
    //permissionSet.AddPermission(new EnvironmentPermission(PermissionState.Unrestricted));
    //permissionSet.Assert();  // BlessedAssert:
    //try
    {
        HKEY regkey = HKEY_LOCAL_MACHINE;
        HKEY recognizerKey = nullptr;
        ::RegOpenKeyExA(regkey, GestureRecognizerPath, 0, 0, &recognizerKey);
        if (recognizerKey != nullptr)
        {
            //try
            {
                // Try to read the recognizer path subkey
                //path = recognizerKey.GetValue(GestureRecognizerValueName) as string;
                LONG cPath = sizeof(path);
                ::RegQueryValueA(recognizerKey, GestureRecognizerValueName, path, &cPath);
                ::RegCloseKey(recognizerKey);
                if (path[0] == 0)
                {
                    return false;
                }
            }
            //finally
            //{
            //    recognizerKey.Close();
            //}
        }
        else
        {
            // we couldn't find the path in the registry
            // no key to close
            return false;
        }
    }
    //finally
    //{
    //    CodeAccessPermission.RevertAssert();
    //}

    if (path[0] != 0)
    {
        HMODULE hModule = ::LoadLibraryA(path);

        // Check whether GetAlternateList exists in the loaded Dll.
        s_GetAlternateListExists = false;
        if ( hModule != nullptr )
        {
            s_GetAlternateListExists = ::GetProcAddress(
                hModule, "GetAlternateList") != nullptr ?
                true : false;
            if (s_GetAlternateListExists) {
                GetAlternateList = GetProcAddressT(hModule, "GetAlternateList", GetAlternateList);
                GetString = GetProcAddressT(hModule, "GetString", GetString);
                GetConfidenceLevel = GetProcAddressT(hModule, "GetConfidenceLevel", GetConfidenceLevel);
                DestroyAlternate = GetProcAddressT(hModule, "DestroyAlternate", DestroyAlternate);
            }
        }

        return hModule != nullptr ? true : false;
    }
    return false; //path was nullptr
}

/// <summary>
/// Set the enabled gestures.
/// This method is called from the SetEnabledGestures method.
/// </summary>
/// <SecurityNote>
///     Critical: Calls a critical pinvoke
///                 NativeRecognizer.UnsafeNativeMethods.SetEnabledUnicodeRanges
/// </SecurityNote>
//[SecurityCritical]
int NativeRecognizer::SetEnabledGestures(HRECOCONTEXT recContext, QList<ApplicationGesture> enabledGestures)
{
    Debug::Assert(recContext != nullptr);

    // NOTICE-2005/01/11-WAYNEZEN,
    // The following usage was copied from drivers\tablet\recognition\ink\core\twister\src\wispapis.c
    // SetEnabledUnicodeRanges
    //      Set ranges of gestures enabled in this recognition context
    //      The behavior of this function is the following:
    //          (a) (A part of) One of the requested ranges lies outside
    //              gesture interval---currently  [GESTURE_NULL, GESTURE_NULL + 255)
    //              return E_UNEXPECTED and keep the previously set ranges
    //          (b) All requested ranges are within the gesture interval but
    //              some of them are not supported:
    //              return S_TRUNCATED and set those requested gestures that are
    //              supported (possibly an empty set)
    //          (c) All requested gestures are supported
    //              return S_OK and set all requested gestures.
    //      Note:  An easy way to set all supported gestures as enabled is to use
    //              SetEnabledUnicodeRanges() with one range=(GESTURE_NULL,255).

    // Enabel gestures
    int cRanges = ( enabledGestures.size() );

    QVector<CHARACTER_RANGE> charRanges(cRanges);

    if ( cRanges == 1 && enabledGestures[0] == ApplicationGesture::AllGestures )
    {
        charRanges[0].cChars = MAX_GESTURE_COUNT;
        charRanges[0].wcLow = GESTURE_NULL;
    }
    else
    {
        for ( int i = 0; i < cRanges; i++ )
        {
            charRanges[i].cChars = 1;
            charRanges[i].wcLow = static_cast<ushort>( enabledGestures[i] );
        }
    }
    int hr = ::SetEnabledUnicodeRanges(recContext, static_cast<ULONG>(cRanges), &charRanges[0]);
    return hr;
}

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
int NativeRecognizer::AddStrokes(HRECOCONTEXT recContext, QSharedPointer<StrokeCollection> strokes)
{
    Debug::Assert(recContext != nullptr);

    int hr;

    for ( QSharedPointer<Stroke> stroke : *strokes )
    {
        PACKET_DESCRIPTION packetDescription;
        QVector<int> packets;
        //try
        {
            int countOfBytes;
            XFORM xForm;
            GetPacketData(stroke, packetDescription, countOfBytes, packets, xForm);
            if (packets.isEmpty())
            {
                return -2147483640; //E_FAIL - 0x80000008.  We never raise this in an exception
            }

            hr = ::AddStroke(recContext, &packetDescription, static_cast<ULONG>(countOfBytes), reinterpret_cast<BYTE*>(&packets[0]), &xForm);
            if ( FAILED(hr) )
            {
                // Return from here. The finally block will free the memory and report the error properly.
                return hr;
            }
        }
        //finally
        {
            // Release the resources in the finally block
            ReleaseResourcesinPacketDescription(packetDescription, packets);
        }
    }

    return ::EndInkInput(recContext);

}

/// <summary>
/// Retrieve the packet description, packets data and XFORM which is the information the native recognizer needs.
/// The method is called from AddStrokes.
/// </summary>
/// <SecurityNote>
///     Critical: Contains unsafe code
/// </SecurityNote>
//[SecurityCritical]
void NativeRecognizer::GetPacketData
(
    QSharedPointer<Stroke> stroke,
    PACKET_DESCRIPTION& packetDescription,
    int& countOfBytes,
    QVector<int>& packets,
    XFORM& xForm
)
{
    int i;
    countOfBytes = 0;
    //packets = nullptr;
    //packetDescription = new PACKET_DESCRIPTION;
    //QMatrix matrix = QMatrix(1, 0, 0, 1, 0, 0);
    //xForm = new XFORM((float)(matrix.M11), (float)(matrix.M12), (float)(matrix.M21),
    //                                (float)(matrix.M22), (float)(matrix.OffsetX), (float)(matrix.OffsetY));
    xForm = XFORM{1, 0, 0, 1, 0, 0};

    QSharedPointer<StylusPointCollection> stylusPoints = stroke->StylusPoints();
    if (stylusPoints->size() == 0)
    {
        return; //we'll fail when the calling routine sees that packets is IntPtr.Zer
    }

    if (stylusPoints->Description()->PropertyCount() > StylusPointDescription::RequiredCountOfProperties)
    {
        //
        // reformat to X, Y, P
        //
        QSharedPointer<StylusPointDescription> reformatDescription(new StylusPointDescription(
                    {
                        StylusPointPropertyInfo(StylusPointProperties::X),
                        StylusPointPropertyInfo(StylusPointProperties::Y),
                        stylusPoints->Description()->GetPropertyInfo(StylusPointProperties::NormalPressure)
        }));
        stylusPoints = stylusPoints->Reformat(reformatDescription);
    }

    //
    // now make sure we only take a finite amount of data for the stroke
    //
    if (stylusPoints->size() > MaxStylusPoints)
    {
        stylusPoints = stylusPoints->Clone(MaxStylusPoints);
    }

    QVector<QUuid> propertyGuids = {  StylusPointPropertyIds::X, //required index for SPD
                                        StylusPointPropertyIds::Y, //required index for SPD
                                        StylusPointPropertyIds::NormalPressure}; //required index for SPD

    Debug::Assert(stylusPoints != nullptr);
    Debug::Assert(propertyGuids.size() == StylusPointDescription::RequiredCountOfProperties);

    // Get the packet description
    packetDescription.cbPacketSize = static_cast<ULONG>(propertyGuids.size() * static_cast<int>(sizeof(ULONG)));
    packetDescription.cPacketProperties = static_cast<ULONG>(propertyGuids.size());

    //
    // use X, Y defaults for metrics, sometimes mouse metrics can be bogus
    // always use NormalPressure metrics, though.
    //
    QVector<StylusPointPropertyInfo> infosToUse(StylusPointDescription::RequiredCountOfProperties);
    infosToUse[StylusPointDescription::RequiredXIndex] = StylusPointPropertyInfoDefaults::X;
    infosToUse[StylusPointDescription::RequiredYIndex] = StylusPointPropertyInfoDefaults::Y;
    infosToUse[StylusPointDescription::RequiredPressureIndex] =
        stylusPoints->Description()->GetPropertyInfo(StylusPointProperties::NormalPressure);

    PACKET_PROPERTY* packetProperties = new PACKET_PROPERTY[packetDescription.cPacketProperties];

    StylusPointPropertyInfo propertyInfo;
    for ( i = 0; i < static_cast<int>(packetDescription.cPacketProperties); i++ )
    {
        packetProperties[i].guid = propertyGuids[i];
        propertyInfo = infosToUse[i];

        PROPERTY_METRICS& propertyMetrics = packetProperties[i].PropertyMetrics;
        propertyMetrics.nLogicalMin = propertyInfo.Minimum();
        propertyMetrics.nLogicalMax = propertyInfo.Maximum();
        propertyMetrics.Units = static_cast<PROPERTY_UNITS>(propertyInfo.Unit());
        propertyMetrics.fResolution = propertyInfo.Resolution();
        //packetProperties[i].PropertyMetrics = propertyMetrics;
    }

    //unsafe
    {
        //int allocationSize = (int)(Marshal.SizeOf(typeof(PACKET_PROPERTY)) * packetDescription.cPacketProperties);
        //packetDescription->pPacketProperties = Marshal.AllocCoTaskMem(allocationSize);
        //PACKET_PROPERTY* pPacketProperty =
        //    (PACKET_PROPERTY*)(packetDescription.pPacketProperties.ToPointer());
        //PACKET_PROPERTY* pElement = pPacketProperty;
        //for ( i = 0 ; i < packetDescription.cPacketProperties ; i ++ )
        //{
        //    Marshal.StructureToPtr(packetProperties[i], new IntPtr(pElement), false);
        //    pElement++;
        //}
        packetDescription.pPacketProperties = packetProperties;
    }

    // Get packet data
    QVector<int> rawPackets = stylusPoints->ToHiMetricArray();
    int packetCount = rawPackets.size();
    if (packetCount != 0)
    {
        countOfBytes = packetCount * static_cast<int>(sizeof(int));
        //packets = Marshal.AllocCoTaskMem(countOfBytes);
        //Marshal.Copy(rawPackets, 0, packets, packetCount);
        packets = rawPackets;
    }
}

/// <summary>
/// Release the memory blocks which has been created for mashalling purpose.
/// The method is called from AddStrokes.
/// </summary>
/// <SecurityNote>
///     Critical: Calls unsafe code, requires UnmanageCode permission
/// </SecurityNote>
//[SecurityCritical]
void NativeRecognizer::ReleaseResourcesinPacketDescription(PACKET_DESCRIPTION& pd, QVector<int>& packets)
{
    (void) packets;
    delete [] pd.pPacketProperties;
    /*
    if ( pd.pPacketProperties != IntPtr.Zero )
    {
        unsafe
        {
            PACKET_PROPERTY* pPacketProperty =
                (PACKET_PROPERTY*)( pd.pPacketProperties.ToPointer( ) );
            PACKET_PROPERTY* pElement = pPacketProperty;

            for ( int i = 0; i < pd.cPacketProperties; i++ )
            {
                Marshal.DestroyStructure(new IntPtr(pElement), typeof(PACKET_PROPERTY));
                pElement++;
            }
        }
        Marshal.FreeCoTaskMem(pd.pPacketProperties);
        pd.pPacketProperties = IntPtr.Zero;
    }

    if ( pd.pQUuidButtons != IntPtr.Zero )
    {
        Marshal.FreeCoTaskMem(pd.pQUuidButtons);
        pd.pQUuidButtons = IntPtr.Zero;
    }

    if ( packets != IntPtr.Zero )
    {
        Marshal.FreeCoTaskMem(packets);
        packets = IntPtr.Zero;
    }
    */
}

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
QList<GestureRecognitionResult> NativeRecognizer::InvokeGetAlternateList()
{
    QList<GestureRecognitionResult> recResults;
    int hr = 0;

    RECO_RANGE recoRange;
    recoRange.iwcBegin = 0;
    recoRange.cCount = 1;
    ULONG countOfAlternates = IRAS_DefaultCount;
    HRECOALT pRecoAlternates[IRAS_DefaultCount];

    //try
    {
        hr = GetAlternateList(_hContext, &recoRange, &countOfAlternates, pRecoAlternates, ALT_BREAKS_SAME);

        if ( SUCCEEDED(hr) && countOfAlternates != 0 )
        {
            QList<GestureRecognitionResult> resultList;

            for ( int i = 0; i < static_cast<int>(countOfAlternates); i++ )
            {
                ULONG size = 1; // length of string == 1 since gesture id is a single WCHAR
                WCHAR recoString[1];
                CONFIDENCE_LEVEL confidenceLevel;

                if ( FAILED(GetString(pRecoAlternates[i], &recoRange, &size, recoString))
                    || FAILED(GetConfidenceLevel(pRecoAlternates[i], &recoRange, &confidenceLevel)) )
                {
                    // Fail to retrieve the reco result, skip this one
                    continue;
                }

                ApplicationGesture gesture = static_cast<ApplicationGesture>(recoString[0]);
                Debug::Assert(ApplicationGestureHelper::IsDefined(gesture));
                if (ApplicationGestureHelper::IsDefined(gesture))
                {
                    resultList.append(GestureRecognitionResult(static_cast<RecognitionConfidence>(confidenceLevel), gesture));
                }
            }

            recResults = resultList;
        }
    }
    //finally
    {
        // Destroy the alternates
        for ( int i = 0; i < static_cast<int>(countOfAlternates); i++ )
        {
            if (pRecoAlternates[i] != nullptr)
            {
                //#pragma warning suppress 6031, 56031 // Return value ignored on purpose.
                DestroyAlternate(pRecoAlternates[i]);
                pRecoAlternates[i] = nullptr;
            }
        }
    }

    return recResults;
}

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
QList<GestureRecognitionResult> NativeRecognizer::InvokeGetLatticePtr()
{
    QList<GestureRecognitionResult> recResults;

//            int hr = 0;
    RECO_LATTICE* ptr = nullptr;

    // NOTICE-2005/07/11-WAYNEZEN,
    // There is no need to free the returned the structure.
    // The memory will be released when ResetContext, which is invoked in the callee - Recognize, is called.
    if ( SUCCEEDED(
        ::GetLatticePtr(
        _hContext, &ptr)) )
    {
        //unsafe
        {
            RECO_LATTICE* pRecoLattice = ptr;

            uint bestResultColumnCount = pRecoLattice->ulBestResultColumnCount;
            Debug::Assert(!(bestResultColumnCount != 0 && pRecoLattice->pLatticeColumns == nullptr), "Invalid results!");
            if ( bestResultColumnCount > 0 && pRecoLattice->pLatticeColumns != nullptr )
            {
                QList<GestureRecognitionResult> resultList;

                RECO_LATTICE_COLUMN* pLatticeColumns = (pRecoLattice->pLatticeColumns);
                ULONG* pulBestResultColumns = (pRecoLattice->pulBestResultColumns);

                for ( uint i = 0; i < bestResultColumnCount; i++ )
                {
                    ULONG column = pulBestResultColumns[i];
                    RECO_LATTICE_COLUMN recoColumn = pLatticeColumns[column];

                    Debug::Assert(0 < recoColumn.cLatticeElements, "Invalid results!");

                    for ( int j = 0; j < static_cast<int>(recoColumn.cLatticeElements); j++ )
                    {
                        RECO_LATTICE_ELEMENT recoElement = recoColumn.pLatticeElements[j];

                        Debug::Assert((recoElement.type) == RECO_TYPE_WCHAR, "The Application gesture has to be WCHAR type" );

                        if ( (recoElement.type) == RECO_TYPE_WCHAR )
                        {
                            // Retrieve the confidence lever
                            RecognitionConfidence confidenceLevel = RecognitionConfidence::Poor;

                            RECO_LATTICE_PROPERTIES recoProperties = recoElement.epProp;

                            uint propertyCount = recoProperties.cProperties;
                            RECO_LATTICE_PROPERTY** apProps = recoProperties.apProps;
                            for ( int k = 0; k < static_cast<int>(propertyCount); k++ )
                            {
                                RECO_LATTICE_PROPERTY* pProps = apProps[k];
                                if ( pProps->guidProperty == GUID_CONFIDENCELEVEL )
                                {
                                    Debug::Assert(pProps->cbPropertyValue == sizeof(uint) / sizeof(byte));
                                    RecognitionConfidence level = static_cast<RecognitionConfidence>(pProps->pPropertyValue[0]);
                                    if ( level >= RecognitionConfidence::Strong && level <= RecognitionConfidence::Poor )
                                    {
                                        confidenceLevel = level;
                                    }

                                    break;
                                }
                            }

                            ApplicationGesture gesture = static_cast<ApplicationGesture>(*recoElement.pData);
                            Debug::Assert(ApplicationGestureHelper::IsDefined(gesture));
                            if (ApplicationGestureHelper::IsDefined(gesture))
                            {
                                // Get the gesture result
                                resultList.append(GestureRecognitionResult(confidenceLevel,gesture));
                            }
                        }
                    }

                }

                recResults = resultList;
            }
        }
    }

    return recResults;
}

//#endregion Methods


/// <summary>
/// RecognizerHandle is a property. But it's a SafeHandle.
/// So, we don't have to worry about releasing the handle since RecognizerSafeHandle when there is no reference on it.
/// </summary>
/// <SecurityNote>
///     Critical: Calls a SecurityCritical pinvoke and accesses SecurityCritical fields
/// </SecurityNote>
HRECOGNIZER NativeRecognizer::RecognizerHandleSingleton()
{
    //[SecurityCritical]
    if (s_isSupported && s_hRec == nullptr)
    {
        {
            QMutexLocker lock(&_syncRoot);
            if (s_isSupported && s_hRec == nullptr)
            {
                CLSID id = s_Gesture;
                if (FAILED(::CreateRecognizer(&id, &s_hRec)))
                {
                    s_hRec = nullptr;
                }
            }
        }
    }

    return s_hRec;
}

// The QUuid has been copied from public\internal\drivers\inc\tpcQUuid.h
//// {7DFE11A7-FB5D-4958-8765-154ADF0D833F}
//DEFINE_QUuid(QUuid_CONFIDENCELEVEL, 0x7dfe11a7, 0xfb5d, 0x4958, 0x87, 0x65, 0x15, 0x4a, 0xdf, 0xd, 0x83, 0x3f);
const QUuid NativeRecognizer::GUID_CONFIDENCELEVEL = QUuid("{7DFE11A7-FB5D-4958-8765-154ADF0D833F}");


QMutex     NativeRecognizer::_syncRoot;

/// <summary>
/// All NativeRecognizer share a single handle to the recognizer
/// </summary>
/// <SecurityNote>
///     Critical: The SecurityCritical handle
/// </SecurityNote>
//[SecurityCritical]
HRECOGNIZER NativeRecognizer::s_hRec;

/// <summary>
/// The QUuid of the GestureRecognizer used for registry lookup
/// </summary>
QUuid        NativeRecognizer::s_Gesture = QUuid(GestureRecognizerGuid);

/// <summary>
/// can we load the recognizer?
/// </summary>
bool NativeRecognizer::s_isSupported;

/// <summary>
/// A flag indicates whether we can find the entry point of
/// GetAlternateList function in mshwgst.dll
/// </summary>
bool NativeRecognizer::s_GetAlternateListExists;


HRESULT (WINAPI *NativeRecognizer::GetAlternateList)(HRECOCONTEXT hrc, RECO_RANGE* pRecoRange, ULONG*pcAlt, HRECOALT*phrcalt, ALT_BREAKS iBreak) = nullptr;
HRESULT (WINAPI *NativeRecognizer::GetString)(HRECOALT hrcalt, RECO_RANGE *pRecoRange, ULONG* pcSize, WCHAR* pwcString) = nullptr;
HRESULT (WINAPI *NativeRecognizer::GetConfidenceLevel)(HRECOALT hrcalt, RECO_RANGE* pRecoRange, CONFIDENCE_LEVEL* pcl) = nullptr;
HRESULT (WINAPI *NativeRecognizer::DestroyAlternate)(HRECOALT hrcalt) = nullptr;
