#include "inkcanvasandroid.h"

#include <jni.h>

#include <Windows/point.h>

#include <Collections/Generic/array.h>

#include <Windows/Ink/stroke.h>

#include <Windows/Media/streamgeometry.h>

INKCANVAS_BEGIN_NAMESPACE

static jclass sc_RuntimeError = nullptr;
static jfieldID sf_PointF_X = nullptr;
static jfieldID sf_PointF_Y = nullptr;
static jmethodID sm_Matrix_getValues = nullptr;
static jmethodID sm_RectF_set = nullptr;

extern JNIEnv * s_env;
JNIEnv * s_env = nullptr;

INKCANVAS_END_NAMESPACE

INKCANVAS_USE_NAMESPACE;

typedef struct {
    char const *name;
    char const *signature;
    void *fnPtr;
} JNINativeMethod2;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*)
{
    JNIEnv *env = nullptr;
    int status = vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    if (status != JNI_OK)
        return status;
    // PointF fields
    jclass clazzPointF = env->FindClass("android/graphics/PointF");
    if (clazzPointF == nullptr) {
        return JNI_ERR;
    }
    sf_PointF_X = env->GetFieldID(clazzPointF, "x", "F");
    sf_PointF_Y = env->GetFieldID(clazzPointF, "y", "F");
    // RectF methods
    jclass clazzRectF = env->FindClass("android/graphics/RectF");
    if (clazzRectF == nullptr) {
        return JNI_ERR;
    }
    sm_RectF_set = env->GetMethodID(clazzRectF, "set", "(FFFF)V");
    // Matrix methods
    jclass clazzMatrix = env->FindClass("android/graphics/Matrix");
    if (clazzMatrix == nullptr) {
        return JNI_ERR;
    }
    sm_Matrix_getValues = env->GetMethodID(clazzRectF, "getValues", "([F)V");
    // Stroke methods
    JNINativeMethod2 methods[] = {
        {"create", "([Landroid/graphics/PointF;DBBB)J]", (void*)&createStroke},
        {"clone", "(J)J]", (void*)&cloneStroke},
        {"transform", "(JLandroid/graphics/Matrix;)V]", (void*)&transformStroke},
        {"hitTest", "(JLandroid/graphics/PointF;)B]", (void*)&hitTestStroke},
        {"getGeometry", "(JLandroid/graphics/RectF;)Landroid/graphics/Path;]", (void*)&getStrokeGeometry},
        {"free", "(J)V]", (void*)&freeStroke},
    };
    jclass clazzStroke = env->FindClass("com.tal.inkcanvas.Stroke");
    if (clazzStroke == nullptr) {
        return JNI_ERR;
    }
    status = env->RegisterNatives(clazzStroke, reinterpret_cast<JNINativeMethod*>(methods), sizeof(methods) / sizeof(methods[0]));
    s_env = env;
    return status;
}

INKCANVAS_BEGIN_NAMESPACE

class MyDrawingAttribute : public DrawingAttributes
{
public:
    MyDrawingAttribute(double width, bool fitToCorve, bool ellipseShape)
    {
        SetWidth(width);
        SetHeight(width);
        SetFitToCurve(fitToCorve);
        SetStylusTip(ellipseShape ? StylusTip::Ellipse : StylusTip::Rectangle);
    }
};

static std::vector<std::shared_ptr<Stroke>> strokes(1, nullptr);

#define S(env, stroke) \
    if (stroke >= static_cast<jlong>(strokes.size())) { \
        env->ThrowNew(sc_RuntimeError, "stroke item not found"); \
        return F; \
    } \
    std::shared_ptr<Stroke> s = strokes[static_cast<size_t>(stroke)]; \
    if (s == nullptr) { \
        env->ThrowNew(sc_RuntimeError, "stroke item not found"); \
        return F; \
    }

jlong createStroke(JNIEnv * env, jobject, jobjectArray points, jdouble width, jboolean fitToCorve, jboolean ellipseShape, jboolean addPressure)
{
    std::shared_ptr<DrawingAttributes> da(new MyDrawingAttribute(width, fitToCorve, ellipseShape));
    std::shared_ptr<StylusPointCollection> stylusPoints(
                new StylusPointCollection);
    int c = env->GetArrayLength(points);
    for (int i = 0; i < c; ++i) {
        jobject point = env->GetObjectArrayElement(points, i);
        float x = env->GetFloatField(point, sf_PointF_X);
        float y = env->GetFloatField(point, sf_PointF_Y);
        StylusPoint stylusPoint(static_cast<double>(x), static_cast<double>(y), 0.5 /*pressure*/);
        stylusPoints->Add(stylusPoint);
    }
    if (addPressure) {
        int n = 16;
        if (stylusPoints->Count() > n) {
            for (int i = 1; i < n; ++i) {
                int m = stylusPoints->Count() + i - n;
                StylusPoint point = (*stylusPoints)[m];
                float d = static_cast<float>(i) / static_cast<float>(n);
                point.SetPressureFactor(point.PressureFactor() * (1.0f - d * d));
                stylusPoints->SetItem(m, point);
            }
            --n;
        } else {
            n = 0;
        }
    }
    std::shared_ptr<Stroke> s(new Stroke(stylusPoints, da));
    auto iter = std::find(strokes.begin() + 1, strokes.end(), nullptr);
    if (iter == strokes.end())
        iter = strokes.insert(iter, s);
    else
        *iter = s;
    return std::distance(strokes.begin(), iter);
}

jlong cloneStroke(JNIEnv * env, jobject, jlong stroke)
{
#undef F
#define F 0
    S(env, stroke)
    s = s->Clone();
    auto iter = std::find(strokes.begin() + 1, strokes.end(), nullptr);
    if (iter == strokes.end())
        iter = strokes.insert(iter, s);
    else
        *iter = s;
    return std::distance(strokes.begin(), iter);
}

void transformStroke(JNIEnv * env, jobject, jlong stroke, jobject matrix)
{
#undef F
#define F
    S(env, stroke)
    jfloatArray matrixValues = env->NewFloatArray(9);
    env->CallVoidMethod(matrix, sm_Matrix_getValues, matrixValues);
    jfloat* matrixValues2 = env->GetFloatArrayElements(matrixValues, nullptr);
#define V(i) static_cast<double>(matrixValues2[i])
    Matrix matrix2(V(0), V(1), V(3), V(4), V(6), V(7));
    env->ReleaseFloatArrayElements(matrixValues, matrixValues2, JNI_ABORT);
    s->Transform(matrix2, false);
}

jboolean hitTestStroke(JNIEnv * env, jobject, jlong stroke, jobject point)
{
#undef F
#define F false
    S(env, stroke)
    float x = env->GetFloatField(point, sf_PointF_X);
    float y = env->GetFloatField(point, sf_PointF_Y);
    return s->HitTest(Point(static_cast<double>(x), static_cast<double>(y)));
}

jobject getStrokeGeometry(JNIEnv * env, jobject, jlong stroke, jobject bounds)
{
#undef F
#define F nullptr
    S(env, stroke)
    s->GetGeometry();
    void * path = static_cast<StreamGeometry*>(s->GetGeometry())->path();
    if (bounds) {
        Rect r = s->GetBounds();
        env->CallVoidMethod(bounds, sm_RectF_set, r.X(), r.Y(),
                            r.Right(), r.Bottom());
    }
    return static_cast<jobject>(path);
}

void freeStroke(JNIEnv *env, jobject, jlong stroke)
{
#undef F
#define F
    S(env, stroke)
    s.reset();
}

INKCANVAS_END_NAMESPACE
