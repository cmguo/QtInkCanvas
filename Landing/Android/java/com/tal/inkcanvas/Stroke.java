package com.tal.inkcanvas;

import android.graphics.Matrix;
import android.graphics.Path;
import android.graphics.PointF;
import android.graphics.RectF;

public class Stroke
{
    private long handle_ = 0;

    static {
        System.loadLibrary("InkCanvasAndroid");
    }

    /*
        if pressures != null, addPressure is ignored
     */
    public Stroke(final PointF[] points, final float[] pressures, final float width, final boolean fitToCorve, final boolean ellipseShape,
            final boolean addPressure) {
        handle_ = create(points, pressures, width, fitToCorve, ellipseShape, addPressure);
    }

    public Stroke(final Stroke o) {
        handle_ = clone(o.handle_);
    }

    public void transform(final Matrix matrix) {
        transform(handle_, matrix);
    }

    public boolean hitTest(final PointF point) {
        return hitTest(handle_, point);
    }

    public Path getGeometry(final RectF bounds) {
        return getGeometry(handle_, bounds);
    }

    @Override
    public void finalize() {
        free(handle_);
        handle_ = 0;
    }

    private native long create(PointF[] points, float[] pressures, float width,
        boolean fitToCorve, boolean ellipseShape, boolean addPressure);

    private native long clone(long handle);

    private native void transform(long handle, Matrix matrix);

    private native boolean hitTest(long handle, PointF point);

    private native Path getGeometry(long handle, RectF bounds);

    private native void free(long handle);
}
