#ifndef CLIPBOARDPROCESSOR_H
#define CLIPBOARDPROCESSOR_H

#include "InkCanvas_global.h"
#include "inkcanvasclipboardformat.h"

#include <QMap>
#include <QList>
#include <QSizeF>
#include <QPointF>
#include <QSharedPointer>
#include <QMetaEnum>

class QMimeData;

// namespace MS.Internal.Ink
INKCANVAS_BEGIN_NAMESPACE

class StrokeCollection;
class InkCanvas;
class UIElement;
class DependencyObjectType;
class ClipboardData;
typedef QMimeData DataObject;

/// <summary>
/// ClipboardProcessor acts as a brige between InkCanvas and various clipboard data formats
/// It provides the functionalies -
///     1. Check the supported data formats in an DataObject
///     2. Copy the selections in an InkCavans to an DataObject
///     3. Create the stroks or frameworkelement array if there is any supported data in an DataObject
/// </summary>
class ClipboardProcessor
{
    //-------------------------------------------------------------------------------
    //
    // Constructors
    //
    //-------------------------------------------------------------------------------

    //#region Constructors
public:
    ClipboardProcessor(InkCanvas& inkCanvas);

    //#endregion Constructors

    //-------------------------------------------------------------------------------
    //
    // Methods
    //
    //-------------------------------------------------------------------------------

    //#region Methods

    /// <summary>
    /// This method returns the bits flag if there are the supported data in the DataObject.
    /// </summary>
    /// <param name="dataObject">The DataObject instance</param>
    /// <returns>The matched clipboard format. Return -1 if there is no recognized format in the data object</returns>
    bool CheckDataFormats(DataObject const * dataObject);

    /// <summary>
    /// The method copies the current selection to the DataObject if there is any
    /// Called by :
    ///             InkCanvas.CopyToDataObject
    /// </summary>
    /// <param name="dataObject">The DataObject instance</param>
    /// <returns>true if there is data being copied. Otherwise return false</returns>
    /// <SecurityNote>
    ///     Critical: This code copies ink content to the clipboard
    ///                 Note the TAS boundary is InkCanvas.CopyToDataObject
    /// </SecurityNote>
    //[SecurityCritical]
    InkCanvasClipboardDataFormats CopySelectedData(DataObject * dataObject);

    /// <summary>
    /// The method returns the Strokes or UIElement array if there is the supported data in the DataObject
    /// </summary>
    /// <param name="dataObject">The DataObject instance</param>
    /// <param name="newStrokes">The strokes which are converted from the data in the DataObject</param>
    /// <param name="newElements">The elements array which are converted from the data in the DataObject</param>
    bool PasteData(DataObject const* dataObject, QSharedPointer<StrokeCollection>& newStrokes, QList<UIElement*>& newElements);

    //#endregion Methods

    QList<InkCanvasClipboardFormat> PreferredFormats();
    void SetPreferredFormats(QList<InkCanvasClipboardFormat> value);



    //-------------------------------------------------------------------------------
    //
    // Methods
    //
    //-------------------------------------------------------------------------------

    //#region Methods

    /// <summary>
    /// Copy the current Selection in XAML format.
    /// Called by :
    ///             CopySelectedData
    /// </summary>
    /// <param name="dataObject"></param>
    /// <param name="strokes"></param>
    /// <param name="elements"></param>
    /// <param name="transform"></param>
    /// <param name="size"></param>
    /// <returns>True if the copy is succeeded</returns>
    /// <SecurityNote>
    ///     Critical:   This code calls CopyToDataObject which is critical.
    ///                 Note the TAS boundary is InkCanvas.CopyToDataObject
    ///
    ///     TreatAsSafe: We only execute this code if the application has UnmanagedCode permission
    /// </SecurityNote>
    //[SecurityCritical, SecurityTreatAsSafe]
    bool CopySelectionInXAML(DataObject* dataObject, QSharedPointer<StrokeCollection> strokes, QList<UIElement*> elements, QMatrix transform, QSizeF size);

    void TearDownInkCanvasContainer(InkCanvas& rootInkCanvas, QSharedPointer<StrokeCollection>& newStrokes, QList<UIElement*>& newElements);

    //#endregion Methods

    //-------------------------------------------------------------------------------
    //
    // Properties
    //
    //-------------------------------------------------------------------------------

    //#region Properties

    InkCanvas& GetInkCanvas()
    {
        return _inkCanvas;
    }

    /// <summary>
    /// A static DependencyObjectType of the GrabHandleAdorner which can be used for quick type checking.
    /// </summary>
    /// <value>An DependencyObjectType object</value>
    static DependencyObjectType* InkCanvasDType();


    //#endregion Properties

    //-------------------------------------------------------------------------------
    //
    // Fields
    //
    //-------------------------------------------------------------------------------

    //#region Fields

private:
    InkCanvas&                       _inkCanvas;
    static DependencyObjectType*     s_InkCanvasDType;

    QMap<InkCanvasClipboardFormat, ClipboardData*> _preferredClipboardData;

    //#endregion Fields

};

INKCANVAS_END_NAMESPACE

#endif // CLIPBOARDPROCESSOR_H
