#ifndef SHAREDPTR_H
#define SHAREDPTR_H

#include "InkCanvas_global.h"

#ifndef INKCANVAS_CORE
#include <QObject>
#include <QSharedPointer>
#include <QPolygonF>
INKCANVAS_BEGIN_NAMESPACE
using SharedPointer = QSharedPointer;
using EnableSharedFromThis = QEnableSharedFromThis;
INKCANVAS_END_NAMESPACE
#else
#include <memory>
INKCANVAS_BEGIN_NAMESPACE
template <typename T> using SharedPointer = std::shared_ptr<T>;
template <typename T> using EnableSharedFromThis = std::enable_shared_from_this<T>;
INKCANVAS_END_NAMESPACE
#endif

#endif // SHAREDPTR_H
