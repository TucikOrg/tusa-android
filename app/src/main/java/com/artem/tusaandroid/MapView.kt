package com.artem.tusaandroid

import android.annotation.SuppressLint
import android.content.Context
import android.graphics.PointF
import android.opengl.GLSurfaceView
import android.view.GestureDetector
import android.view.GestureDetector.SimpleOnGestureListener
import android.view.MotionEvent
import android.view.ScaleGestureDetector
import android.view.ScaleGestureDetector.SimpleOnScaleGestureListener
import android.view.SurfaceHolder
import com.artem.tusaandroid.app.MeAvatarState
import com.artem.tusaandroid.location.LastLocationState

@SuppressLint("ViewConstructor")
class MapView(
    context: Context,
    private val meAvatarState: MeAvatarState,
    private val lastLocationState: LastLocationState
) : GLSurfaceView(context) {
    private var scaleGestureDetector: ScaleGestureDetector? = null
    private var gestureDetector: GestureDetector? = null

    // Зум при запуске от 0 до 19
    private val onAppStartMapZoom = 0

    private val scaleShift = 1.0f
    private val maxScale = 19f + scaleShift
    private var scaleFactor = onAppStartMapZoom + scaleShift
    private var lastFocusX = 0f
    private var lastFocusY = 0f
    private val translate = PointF(0f, 0f)
    private val scaleSpeed = 0.25f

    fun getScaleForMap(): Float {
        return scaleFactor - scaleShift
    }

    init {
        // Чтобы при старте синхронизировать C++ и Java состояния карты
        NativeLibrary.noOpenGlContextInit(resources.assets, getScaleForMap())

        setEGLContextClientVersion(2)
        setEGLConfigChooser(8, 8, 8, 8, 16, 8)
        setRenderer(Renderer(
            resources.assets,
            meAvatarState,
            lastLocationState
        ))

        scaleGestureDetector = ScaleGestureDetector(context, ScaleListener())
        gestureDetector = GestureDetector(context, GestureListener())
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        super.surfaceDestroyed(holder)
        NativeLibrary.onStop()
    }

    @SuppressLint("ClickableViewAccessibility")
    override fun onTouchEvent(event: MotionEvent): Boolean {
        scaleGestureDetector!!.onTouchEvent(event)
        gestureDetector!!.onTouchEvent(event)
        return true
    }

    private inner class ScaleListener : SimpleOnScaleGestureListener() {
        override fun onScale(detector: ScaleGestureDetector): Boolean {
            val detectorScale = detector.scaleFactor
            val realScale = (detectorScale - 1f)

            scaleFactor *= (1 + realScale * scaleSpeed)
            scaleFactor = scaleShift.coerceAtLeast(scaleFactor.coerceAtMost(maxScale))

            NativeLibrary.scale(getScaleForMap())
            return true
        }

        override fun onScaleBegin(detector: ScaleGestureDetector): Boolean {
            lastFocusX = detector.focusX
            lastFocusY = detector.focusY
            return true
        }
    }

    private inner class GestureListener : SimpleOnGestureListener() {
        override fun onScroll(
            e1: MotionEvent?,
            e2: MotionEvent,
            distanceX: Float,
            distanceY: Float
        ): Boolean {
            translate.x -= distanceX
            translate.y -= distanceY
            NativeLibrary.drag(distanceX, distanceY)
            return true
        }

        override fun onDoubleTap(e: MotionEvent): Boolean {
            NativeLibrary.doubleTap()
            return true
        }

        override fun onDown(e: MotionEvent): Boolean {
            NativeLibrary.onDown()
            return true
        }
    }
}