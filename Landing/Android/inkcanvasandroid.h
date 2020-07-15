#ifndef INKCANVASANDROID_H
#define INKCANVASANDROID_H

#include "InkCanvas_global.h"

#include <jni.h>

INKCANVAS_BEGIN_NAMESPACE


jlong createStroke(JNIEnv * env, jobject object, jobjectArray points, jfloatArray pressures,
                   jfloat width, jboolean fitToCorve, jboolean ellipseShape, jboolean addPressure);

jlong cloneStroke(JNIEnv * env, jobject object, jlong stroke);

void transformStroke(JNIEnv * env, jobject object, jlong stroke, jobject matrix);

jboolean hitTestStroke(JNIEnv * env, jobject object, jlong stroke, jobject point);

jobject getStrokeGeometry(JNIEnv * env, jobject object, jlong stroke, jobject bounds);

void freeStroke(JNIEnv * env, jobject object, jlong stroke);

INKCANVAS_END_NAMESPACE

#endif // INKCANVASANDROID_H
