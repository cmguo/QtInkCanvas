#ifndef PENCONTEXTS_H
#define PENCONTEXTS_H

#include "Windows/Input/StylusPlugIns/rawstylusactions.h"
#include "Windows/Input/StylusPlugIns/rawstylusinput.h"

#include <QObject>
#include <QMutex>
#include <QTransform>

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
    QList<StylusPlugInCollection*> stylusPlugIns_;
    QTransform transform_;
    RawStylusActions action_;
    QList<RawStylusInputCustomData> customDatas_;
};

#endif // PENCONTEXTS_H
