#ifndef STYLUSPLUGINCOLLECTION_H
#define STYLUSPLUGINCOLLECTION_H

#include <QList>
#include <QMutex>
#include <QTransform>

class StylusPlugIn;
class UIElement;
class RawStylusInput;
class PenContexts;
class DependencyPropertyChangedEventArgs;
class EventArgs;
class SourceChangedEventArgs;

/// <summary>
/// Collection of StylusPlugIn objects
/// </summary>
/// <remarks>
/// The collection order is based on the order that StylusPlugIn objects are
/// added to the collection via the IList interfaces. The order of the StylusPlugIn
/// objects in the collection is modifiable.
/// Some of the methods are designed to be called from both the App thread and the Pen thread,
/// but some of them are supposed to be called from one thread only. Please look at the
/// comments of each method for such an information.
/// </remarks>
class StylusPlugInCollection : public QObject, public QList<StylusPlugIn*>
{
    Q_OBJECT
public:
   //#region Protected APIs
   /// <summary>
   /// Insert a StylusPlugIn in the collection at a specific index.
   /// This method should be called from the application context only
   /// </summary>
   /// <param name="index">index at which to insert the StylusPlugIn object
   /// <param name="plugIn">StylusPlugIn object to insert, downcast to an object
   void InsertItem(int index, StylusPlugIn* plugIn);

   /// <summary>
   /// Remove all the StylusPlugIn objects from the collection.
   /// This method should be called from the application context only.
   /// </summary>
   void ClearItems();

   /// <summary>
   /// Remove the StylusPlugIn in the collection at the specified index.
   /// This method should be called from the application context only.
   /// </summary>
   /// <param name="index">
   void RemoveItem(int index);

   /// <summary>
   /// Indexer to retrieve/set a StylusPlugIn at a given index in the collection
   /// Accessible from both the real time context and application context.
   /// </summary>
   void SetItem(int index, StylusPlugIn* plugIn);

   //#endregion

   //#region Internal APIs
public:
   /// <summary>
   /// Constructor
   /// </summary>
   /// <param name="element">
   StylusPlugInCollection(UIElement* element = nullptr);

   /// <summary>
   /// Get the UIElement
   /// This method is called from the real-time context.
   /// </summary>
    UIElement* Element();

   /// <summary>
   /// Update the rectangular bound of the element
   /// This method is called from the application context.
   /// </summary>
   void UpdateRect();

   /// <summary>
   /// Check whether a point hits the element
   /// This method is called from the real-time context.
   /// </summary>
   /// <param name="pt">a point to check
   /// <returns>true if the point is within the bound of the element; false otherwise</returns>
   bool IsHit(QPointF pt);

   /// <summary>
   /// Get the transform matrix from the root visual to the current UIElement
   /// This method is called from the real-time context.
   /// </summary>
   QTransform ViewToElement();

   /// <summary>
   /// Get the current rect for the Element that the StylusPlugInCollection is attached to.
   /// May be empty rect if plug in is not in tree.
   /// </summary>
   QRectF Rect();

   /// <summary>
   /// Get the current rect for the Element that the StylusPlugInCollection is attached to.
   /// May be empty rect if plug in is not in tree.
   /// </summary>
   /// <securitynote>
   /// Critical - Accesses SecurityCritical data _penContexts.
   /// TreatAsSafe - Just returns if _pencontexts is null.  No data goes in or out.  Knowing
   ///               the fact that you can recieve real time input is something that is safe
   ///               to know and we want to expose.
   /// </securitynote>
   bool IsActiveForInput();


   /// <securitynote>
   /// Critical - Accesses SecurityCritical data _penContexts.
   /// TreatAsSafe - The [....] object on the _penContexts object is not considered security
   ///                 critical data.  It is already internally exposed directly on the
   ///                 PenContexts object.
   /// </securitynote>
   QMutex& PenContextsSyncRoot();


   /// <summary>
   /// Fire the Enter notification.
   /// This method is called from pen threads and app thread.
   /// </summary>
   void FireEnterLeave(bool isEnter, RawStylusInput& rawStylusInput, bool confirmed);

   /// <summary>
   /// Fire RawStylusInputEvent for all the StylusPlugIns
   /// This method is called from the real-time context (pen thread) only
   /// </summary>
   /// <param name="args">
   void FireRawStylusInput(RawStylusInput& args);


   /// <securitynote>
   ///     Critical:  Accesses critical member _penContexts.
   /// </securitynote>
   PenContexts* GetPenContexts();

   //#endregion

   //#region APIs

private:
   /// <summary>
   /// Add this StylusPlugInCollection to the StylusPlugInCollectionList when it the first
   /// element is added.
   /// </summary>
   /// <securitynote>
   /// Critical - Presentation source access
   /// TreatAsSafe: - PresentationSource makes a SecurityDemand
   ///                    - no data handed out or accepted
   ///                    - called by Add and Insert
   /// </securitynote>
   void EnsureEventsHooked();

   /// <summary>
   /// Remove this StylusPlugInCollection from the StylusPlugInCollectionList when it the last
   /// element is removed for this collection.
   /// </summary>
   void EnsureEventsAndPenContextsUnhooked();

   void OnIsEnabledChanged();

   void OnIsVisibleChanged();

   void OnIsHitTestVisibleChanged();

   void OnRenderTransformChanged(EventArgs& e);

   void OnSourceChanged(SourceChangedEventArgs& e);

   void OnLayoutUpdated(EventArgs& e);

   // On app ui dispatcher
   /// <securitynote>
   /// Critical - Presentation source access
   ///            Calls SecurityCritical routines PresentationSource.CriticalFromVisual and
   ///            HwndSource.CriticalHandle.
   /// TreatAsSafe:
   ///          - no data handed out or accepted
   /// </securitynote>
   void UpdatePenContextsState();

   /// <securitynote>
   /// Critical - _penContexts access
   /// TreatAsSafe:
   ///          - no data handed out or accepted
   /// </securitynote>
   void UnhookPenContexts();

   //#endregion
private:
   //#region Fields
   UIElement* _element;
   QRectF _rc; // In window root measured units
   QTransform _viewToElement;

   QTransform _lastRenderTransform;

   // Note that this is only set when the Element is in a state to receive input (visible,enabled,in tree).
   /// <securitynote>
   ///     Critical to prevent accidental spread to transparent code
   /// </securitynote>
   PenContexts* _penContexts = nullptr;

   //DependencyPropertyChangedEventHandler _isEnabledChangedEventHandler;
   //DependencyPropertyChangedEventHandler _isVisibleChangedEventHandler;
   //DependencyPropertyChangedEventHandler _isHitTestVisibleChangedEventHandler;
   //EventHandler _renderTransformChangedEventHandler;
   //SourceChangedEventHandler _sourceChangedEventHandler;
   //EventHandler _layoutChangedEventHandler;
   //#endregion
};

Q_DECLARE_METATYPE(StylusPlugInCollection*)

#endif // STYLUSPLUGINCOLLECTION_H
