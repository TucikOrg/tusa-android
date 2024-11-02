package com.artem.tusaandroid

import android.annotation.SuppressLint
import android.content.Context
import android.opengl.GLSurfaceView
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

@SuppressLint("ViewConstructor")
class MapView(
    context: Context,
    private val meAvatarState: MeAvatarState,
    private val lastLocationState: LastLocationState,
    private val requestTile: RequestTile,
    private val socketListener: SocketListener,
    private val avatarState: AvatarState
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
            NativeLibrary.scale(detector.scaleFactor)
            return true
        }

        override fun onScaleBegin(detector: ScaleGestureDetector): Boolean {
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