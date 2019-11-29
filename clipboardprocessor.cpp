#include "clipboardprocessor.h"
#include "inkcanvas.h"
#include "strokecollection.h"
#include "isfclipboarddata.h"
#include "inkcanvasselection.h"
#include "textclipboarddata.h"

//-------------------------------------------------------------------------------
//
// Constructors
//
//-------------------------------------------------------------------------------

//#region Constructors
ClipboardProcessor::ClipboardProcessor(InkCanvas& inkCanvas)
    : _inkCanvas(inkCanvas)
{
    //if ( inkCanvas == nullptr )
    //{
    //    throw std::exception("inkCanvas");
    //}

    //_inkCanvas = inkCanvas;

    // Create our default preferred list - Only InkCanvasClipboardFormat::Isf is supported.
    //_preferredClipboardData = new Dictionary<InkCanvasClipboardFormat, ClipboardData>();
    _preferredClipboardData[InkCanvasClipboardFormat::InkSerializedFormat] = new ISFClipboardData();

}

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
bool ClipboardProcessor::CheckDataFormats(DataObject const * dataObject)
{
    //Debug.Asert(dataObject != nullptr && _preferredClipboardData!= nullptr);

    for ( auto pair : _preferredClipboardData )
    {
        if ( pair->CanPaste(dataObject) )
        {
            return true;
        }
    }

    return false;
}

/// <summary>
/// The method copies the current selection to the DataObject if there is any
/// Called by :
///             GetInkCanvas().CopyToDataObject
/// </summary>
/// <param name="dataObject">The DataObject instance</param>
/// <returns>true if there is data being copied. Otherwise return false</returns>
/// <SecurityNote>
///     Critical: This code copies ink content to the clipboard
///                 Note the TAS boundary is GetInkCanvas().CopyToDataObject
/// </SecurityNote>
//[SecurityCritical]
InkCanvasClipboardDataFormats ClipboardProcessor::CopySelectedData(DataObject * dataObject)
{
    InkCanvasClipboardDataFormats copiedDataFormat = InkCanvasClipboardDataFormat::None;
    InkCanvasSelection& inkCanvasSelection = GetInkCanvas().GetInkCanvasSelection();

    QSharedPointer<StrokeCollection> strokes = inkCanvasSelection.SelectedStrokes();
    if (strokes->size() > 1)
    {
        // NTRAID#WINDOWS-1541633-2006/03/03-SAMGEO,
        // order the strokes so they are in the correct z-order
        // they appear in on the InkCanvas, or else they will be inconsistent
        // if copied / pasted
        QSharedPointer<StrokeCollection> orderedStrokes(new StrokeCollection());
        QSharedPointer<StrokeCollection> inkCanvasStrokes = GetInkCanvas().Strokes(); //cache to avoid multiple property gets
        for (int i = 0; i < inkCanvasStrokes->size() && strokes->size() != orderedStrokes->size(); i++)
        {
            for (int j = 0; j < strokes->size(); j++)
            {
                if ((*inkCanvasStrokes)[i] == (*strokes)[j])
                {
                    orderedStrokes->append((*strokes)[j]);
                    break;
                }
            }
        }

        //Debug.Asert(inkCanvasSelection.SelectedStrokes.Count == orderedStrokes.Count);
        //Make a copy collection since we will alter the transform before copying the data.
        strokes = orderedStrokes->Clone();
    }
    else
    {
        //we only have zero or one stroke so we don't need to order, but we
        //do need to clone.
        strokes = strokes->Clone();
    }

    QList<UIElement*> elements(inkCanvasSelection.SelectedElements());
    QRectF bounds = inkCanvasSelection.SelectionBounds();

    // Now copy the selection in the below orders.
    if ( strokes->size() != 0 || elements.size() != 0 )
    {
        // NTRAID-WINDOWS#1412097-2005/12/08-WAYNEZEN,
        // The selection should be translated to the origin (0, 0) related to its bounds.
        // Get the translate transform as a relative bounds.
        QMatrix transform = QMatrix(1, 0, 0, 1, -bounds.left(), -bounds.top());
        //transform.OffsetX = -bounds.Left;
        //transform.OffsetY = -bounds.Top;

        // Add ISF data first.
        if ( strokes->size() != 0 )
        {
            // Transform the strokes first.
            inkCanvasSelection.TransformStrokes(strokes, transform);

            ClipboardData* data = new ISFClipboardData(strokes);
            data->CopyToDataObject(dataObject);
            copiedDataFormat |= InkCanvasClipboardDataFormat::ISF;
        }

        // Then add XAML data.
        if ( CopySelectionInXAML(dataObject, strokes, elements, transform, bounds.size()) )
        {
            // We have to create an InkCanvas as a container and add all the selection to it.
            copiedDataFormat |= InkCanvasClipboardDataFormat::XAML;
        }
    }
    else
    {
        //Debug.Asert(false , "CopySelectData: InkCanvas should have a selection!");
    }

    return copiedDataFormat;
}

/// <summary>
/// The method returns the Strokes or UIElement array if there is the supported data in the DataObject
/// </summary>
/// <param name="dataObject">The DataObject instance</param>
/// <param name="newStrokes">The strokes which are converted from the data in the DataObject</param>
/// <param name="newElements">The elements array which are converted from the data in the DataObject</param>
bool ClipboardProcessor::PasteData(DataObject const* dataObject, QSharedPointer<StrokeCollection>& newStrokes, QList<UIElement*>& newElements)
{
    //Debug.Asert(dataObject != nullptr && _preferredClipboardData!= nullptr);

    // We honor the order in our preferred list.
    for ( auto format : _preferredClipboardData.keys() )
    {
        //InkCanvasClipboardFormat format = pair;
        ClipboardData* data = _preferredClipboardData.value(format);

        if ( data->CanPaste(dataObject) )
        {
            switch ( format )
            {
                case InkCanvasClipboardFormat::Xaml:
                {
                    //XamlClipboardData* xamlData = (XamlClipboardData)data;
                    //xamlData.PasteFromDataObject(dataObject);

                    //QList<UIElement*> elements = xamlData.Elements;

                    //if (elements != nullptr && elements.size() != 0)
                    {
                        // If the Xaml data has been set in an InkCanvas, the top element will be a container GetInkCanvas().
                        // In this case, the new elements will be the children of the container.
                        // Otherwise, the new elements will be whatever data from the data object.
                        //if (elements.size() == 1 && ClipboardProcessor.InkCanvasDType.IsInstanceOfType(elements[0]))
                        //{
                        //    TearDownInkCanvasContainer((InkCanvas)( elements[0] ), newStrokes, newElements);
                        //}
                        //else
                        {
                            // The new elements are the data in the data object.
                    //        newElements = elements;
                        }

                    }
                    break;
                }
                case InkCanvasClipboardFormat::InkSerializedFormat:
                {
                    // Retrieve the stroke data.
                    ISFClipboardData* isfData = static_cast<ISFClipboardData*>(data);
                    isfData->PasteFromDataObject(dataObject);

                    newStrokes = isfData->Strokes();
                    break;
                }
                case InkCanvasClipboardFormat::Text:
                {
                    // Convert the text data in the data object to a text box element.
                    TextClipboardData* textData = static_cast<TextClipboardData*>(data);
                    textData->PasteFromDataObject(dataObject);
                    newElements = textData->Elements();
                    break;
                }
            }

            // Once we've done pasting, just return now.
            return true;
        }
    }

    // Nothing gets pasted.
    return false;
}

//#endregion Methods

QList<InkCanvasClipboardFormat> ClipboardProcessor::PreferredFormats()
{
    //Debug.Asert(_preferredClipboardData != nullptr);
    return  _preferredClipboardData.keys();
}

void ClipboardProcessor::SetPreferredFormats(QList<InkCanvasClipboardFormat> value)
{
    //Debug.Asert(value != nullptr);

    QMap<InkCanvasClipboardFormat, ClipboardData*> preferredData;

    for ( InkCanvasClipboardFormat format : value )
    {
        // If we find the duplicate format in our preferred list, we should just skip it.
        if ( !preferredData.contains(format) )
        {
            ClipboardData* clipboardData = nullptr;
            switch ( format )
            {
                case InkCanvasClipboardFormat::InkSerializedFormat:
                    clipboardData = new ISFClipboardData();
                    break;
                case InkCanvasClipboardFormat::Xaml:
                    //clipboardData = new XamlClipboardData();
                    break;
                case InkCanvasClipboardFormat::Text:
                    clipboardData = new TextClipboardData();
                    break;
                default:
                    throw std::exception("value");
            }

            preferredData.insert(format, clipboardData);
        }
    }

    _preferredClipboardData = preferredData;
}



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
///                 Note the TAS boundary is GetInkCanvas().CopyToDataObject
///
///     TreatAsSafe: We only execute this code if the application has UnmanagedCode permission
/// </SecurityNote>
//[SecurityCritical, SecurityTreatAsSafe]
/*
bool ClipboardProcessor::CopySelectionInXAML(DataObject* dataObject, QSharedPointer<StrokeCollection> strokes, QList<UIElement*> elements, QMatrix transform, QSizeF size)
{
    //NOTE: after meeting with the partial trust team, we have
    //collectively decided to only allow copy / cut of XAML if the caller
    //has unmanagedcode permission, else we silently ignore the XAML
    if (!SecurityHelper.CheckUnmanagedCodePermission())
    {
        return false;
    }
    else
    {

        QSharedPointer<InkCanvas> inkCanvas(new InkCanvas());

        // NOTICE-2005/12/06-WAYNEZEN,
        // We already transform the Strokes in CopySelectedData.
        if (strokes->size() != 0)
        {
            GetInkCanvas().SetStrokes(strokes);
        }

        int elementCount = elements.size();
        if (elementCount != 0)
        {
            InkCanvasSelection& inkCanvasSelection = GetInkCanvas().GetInkCanvasSelection();

            for (int i = 0; i < elementCount; i++)
            {
                // NOTICE-2005/05/05-WAYNEZEN,
                // An element can't be added to two visual trees.
                // So, we cannot add the elements to the new container since they have been added to the current GetInkCanvas().
                // Here we have to do is according to the suggestion from Avalon team -
                //      1. Presist the elements to Xaml
                //      2. Load the xaml to create the new instances of the elements.
                //      3. Add the new instances to the new container.
                string xml;

                try
                {
                    xml = XamlWriter.Save(elements[i]);

                    UIElement* newElement = XamlReader.Load(new XmlTextReader(new StringReader(xml))) as UIElement;
                    ((IAddChild)inkCanvas).AddChild(newElement);

                    // Now we tranform the element.
                    inkCanvasSelection.UpdateElementBounds(elements[i], newElement, transform);
                }
                catch (SecurityException)
                {
                    // If we hit a SecurityException under the PartialTrust, we should just stop generating
                    // the containing GetInkCanvas().
                    inkCanvas = nullptr;
                    break;
                }
            }
        }

        if (inkCanvas != nullptr)
        {
            GetInkCanvas().SetWidth(size.width());
            GetInkCanvas().SetHeight(size.height());

            ClipboardData* data = new XamlClipboardData(new UIElement[] { inkCanvas });

            try
            {
                data.CopyToDataObject(dataObject);
            }
            catch (SecurityException)
            {
                // If we hit a SecurityException under the PartialTrust, we should just fail the copy
                // operation.
                inkCanvas = nullptr;
            }
        }

        return inkCanvas != nullptr;
    }
}
*/

void ClipboardProcessor::TearDownInkCanvasContainer(InkCanvas& rootInkCanvas, QSharedPointer<StrokeCollection>& newStrokes, QList<UIElement*>& newElements)
{
    newStrokes = rootInkCanvas.Strokes();

    if ( rootInkCanvas.Children().size() != 0 )
    {
        QList<UIElement*> children;//(rootGetInkCanvas().Children.Count);
        for (UIElement* uiElement : rootInkCanvas.Children())
        {
            children.append(uiElement);
        }

        // Remove the children for the container
        for( UIElement* child : children )
        {
            rootInkCanvas.Children().removeOne(child);
        }

        // The new elements will be the children.
        newElements = children;
    }

}

//#endregion Methods

//-------------------------------------------------------------------------------
//
// Properties
//
//-------------------------------------------------------------------------------


/// <summary>
/// A static DependencyObjectType of the GrabHandleAdorner which can be used for quick type checking.
/// </summary>
/// <value>An DependencyObjectType object</value>
DependencyObjectType* ClipboardProcessor::InkCanvasDType()
{
    if ( s_InkCanvasDType == nullptr )
    {
        //s_InkCanvasDType = DependencyObjectType.FromSystemTypeInternal(typeof(InkCanvas));
    }

    return s_InkCanvasDType;
}

