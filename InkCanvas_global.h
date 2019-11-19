#ifndef INKCANVAS_GLOBAL_H
#define INKCANVAS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(INKCANVAS_LIBRARY)
#  define INKCANVAS_EXPORT Q_DECL_EXPORT
#else
#  define INKCANVAS_EXPORT Q_DECL_IMPORT
#endif

#endif // INKCANVAS_GLOBAL_H
