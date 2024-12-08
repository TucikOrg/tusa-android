package com.artem.tusaandroid

import android.annotation.SuppressLint
import android.content.Context
import android.opengl.GLSurfaceView
import android.util.Log
import android.view.GestureDetector
import android.view.GestureDetector.SimpleOnGestureListener
import android.view.MotionEvent
import android.view.ScaleGestureDetector
import android.view.ScaleGestureDetector.SimpleOnScaleGestureListener
import android.view.SurfaceHolder
import com.artem.tusaandroid.app.MeAvatarState
import com.artem.tusaandroid.app.avatar.AvatarState
import com.artem.tusaandroid.location.LastLocationState
import com.artem.tusaandroid.socket.SocketListener
import kotlin.math.abs

@SuppressLint("ViewConstructor")
class MapView(
    context: Context,
    private val meAvatarState: MeAvatarState?,
    private val lastLocationState: LastLocationState?,
    private val requestTile: RequestTile,
    private val socketListener: SocketListener?,
    private val avatarState: AvatarState?
) : GLSurfaceView(context) {
    private var scaleGestureDetector: ScaleGestureDetector? = null
    private var gestureDetector: GestureDetector? = null


    init {
        // Настраиваем карту первоначально перед запуском
        NativeLibrary.noOpenGlContextInit(resources.assets, requestTile)

        setEGLContextClientVersion(2)
        setEGLConfigChooser(8, 8, 8, 8, 16, 8)
        setRenderer(Renderer(
            resources.assets,
            meAvatarState,
            lastLocationState,
            socketListener,
            avatarState
        ))
    }

    override fun surfaceCreated(holder: SurfaceHolder) {
        super.surfaceCreated(holder)
        val width = holder.surfaceFrame.width().toFloat()
        val height = holder.surfaceFrame.height().toFloat()

        scaleGestureDetector = ScaleGestureDetector(context, ScaleListener(width, height))
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

    private inner class ScaleListener(
        private var width: Float,
        private var height: Float
    ) : SimpleOnScaleGestureListener() {

        override fun onScale(detector: ScaleGestureDetector): Boolean {
            NativeLibrary.scale(detector.scaleFactor)
            return true
        }

        override fun onScaleBegin(detector: ScaleGestureDetector): Boolean {
            return true
        }
    }

    private inner class GestureListener : SimpleOnGestureListener() {
        private var isDragging = false

        override fun onScroll(
            e1: MotionEvent?,
            e2: MotionEvent,
            distanceX: Float,
            distanceY: Float
        ): Boolean {

            val x = e2.x
            val y = e2.y
            val wb = width - width / 4.0f
            val hb = height / 3.0f
            val scaleDragging = x >= wb && y >= hb && !isDragging && abs(distanceX) < abs(distanceY)
            if (scaleDragging) {
                // это приближение и отдаление если правую нижниюю часть экарана трогать пальцем
                // нормально работает но нужно доработать еще некоторые случаи Пока что эта штука иногда раздражает
                //NativeLibrary.scale(1.0f + distanceY / 300.0f)
                return true
            }

            NativeLibrary.drag(distanceX, distanceY)
            isDragging = true
            return true
        }

        override fun onDoubleTap(e: MotionEvent): Boolean {
            NativeLibrary.doubleTap()
            return true
        }

        override fun onDown(e: MotionEvent): Boolean {
            isDragging = false
            NativeLibrary.onDown()
            return true
        }
    }
}