#ifndef PENCONTEXTS_H
#define PENCONTEXTS_H

#include "Windows/Input/StylusPlugIns/rawstylusactions.h"
#include "Windows/Input/StylusPlugIns/rawstylusinput.h"
#include "Windows/Media/matrix.h"

#include <QObject>
#include <QMutex>

INKCANVAS_BEGIN_NAMESPACE

class StylusPlugInCollection;
class RawStylusInputCustomData;
class UIElement;

class PenContexts : public QObject
{
    Q_OBJECT
public:
    PenContexts(UIElement * element);

    QMutex& SyncRoot();

    void AddStylusPlugInCollection(StylusPlugInCollection* pic);

    void RemoveStylusPlugInCollection(StylusPlugInCollection* pic);

    void FireCustomData();

public:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QMutex mutex_;
    UIElement * element_;
    List<StylusPlugInCollection*> stylusPlugIns_;
    Matrix transform_;
    RawStylusActions action_;
    List<RawStylusInputCustomData> customDatas_;
};

INKCANVAS_END_NAMESPACE

#endif // PENCONTEXTS_H
