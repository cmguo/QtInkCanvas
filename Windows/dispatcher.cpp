#include "Windows/dispatcher.h"
#include "Internal/debug.h"

#include <QThread>
#include <QMutex>
#include <QMap>

INKCANVAS_BEGIN_NAMESPACE

static QMutex dlock;
static QMap<QThread *, Dispatcher*> dispatchers;

Dispatcher* Dispatcher::from(QThread *thread)
{
    QMutexLocker l(&dlock);
    Dispatcher * d = dispatchers.value(thread, nullptr);
    if (d == nullptr) {
        d = new Dispatcher(thread);
        dispatchers.insert(thread, d);
    }
    return d;
}

Dispatcher::Dispatcher(QThread *thread)
{
    eventDispatche_ = thread->eventDispatcher();
}

void Dispatcher::VerifyAccess()
{
    Debug::Assert(eventDispatche_ && QThread::currentThread()->eventDispatcher() == eventDispatche_);
}

bool Dispatcher::CheckAccess()
{
    return eventDispatche_ && QThread::currentThread()->eventDispatcher() == eventDispatche_;
}

void Dispatcher::BeginInvoke(std::function<void (void *)> func, void *data)
{

}

INKCANVAS_END_NAMESPACE
