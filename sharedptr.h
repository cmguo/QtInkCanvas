#ifndef SHAREDPTR_H
#define SHAREDPTR_H

#include "InkCanvas_global.h"

#ifdef INKCANVAS_QT
#include <QObject>
#include <QSharedPointer>
#include <QPolygonF>
INKCANVAS_BEGIN_NAMESPACE
#ifdef Q_COMPILER_TEMPLATE_ALIAS
template <typename T> using SharedPointer = QSharedPointer<T>;
template <typename T> using EnableSharedFromThis = QEnableSharedFromThis<T>;
#else
#define SharedPointer QSharedPointer
#define EnableSharedFromThis QEnableSharedFromThis
#endif
#define shared_from_this sharedFromThis
INKCANVAS_END_NAMESPACE
#else
#include <memory>
INKCANVAS_BEGIN_NAMESPACE
template <typename T> using SharedPointer = std::shared_ptr<T>;
template <typename T> using EnableSharedFromThis = std::enable_shared_from_this<T>;
INKCANVAS_END_NAMESPACE
#endif

#endif // SHAREDPTR_H
