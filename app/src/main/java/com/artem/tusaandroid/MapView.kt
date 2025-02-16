package com.artem.tusaandroid

import android.annotation.SuppressLint
import android.content.Context
import android.opengl.GLSurfaceView
import android.os.Handler
import android.os.Looper
import android.util.Log
import android.view.GestureDetector
import android.view.GestureDetector.SimpleOnGestureListener
import android.view.MotionEvent
import android.view.ScaleGestureDetector
import android.view.ScaleGestureDetector.SimpleOnScaleGestureListener
import android.view.SurfaceHolder
import com.artem.tusaandroid.app.MeAvatarState
import com.artem.tusaandroid.app.avatar.AvatarState
import com.artem.tusaandroid.app.map.UpdateMapTitleState
import com.artem.tusaandroid.app.selected.SelectedState
import com.artem.tusaandroid.app.systemui.SystemUIState
import com.artem.tusaandroid.location.LastLocationState
import com.artem.tusaandroid.location.LocationsState
import com.artem.tusaandroid.socket.SocketListener
import kotlin.math.abs

@SuppressLint("ViewConstructor")
class MapView(
    context: Context,
    private val meAvatarState: MeAvatarState?,
    private val lastLocationState: LastLocationState?,
    private val requestTile: RequestTile,
    private val socketListener: SocketListener?,
    private val avatarState: AvatarState?,
    private val selectedState: SelectedState?,
    private val locationsState: LocationsState?,
    private val systemUIState: SystemUIState?,
    private val updateMapTitleState: UpdateMapTitleState?
) : GLSurfaceView(context) {
    private var scaleGestureDetector: ScaleGestureDetector? = null
    private var gestureDetector: GestureDetector? = null

    private var scaleListener: ScaleListener? = null
    private var gestureListener: GestureListener? = null

    init {
        // Настраиваем карту первоначально перед запуском
        NativeLibrary.create()
        NativeLibrary.noOpenGlContextInit(resources.assets, requestTile)
        systemUIState?.setLight(NativeLibrary.getCameraPos().zoom < 3.0f)

        setEGLContextClientVersion(2)
        setEGLConfigChooser(8, 8, 8, 8, 16, 8)
        setRenderer(Renderer(
            resources.assets,
            meAvatarState,
            lastLocationState,
            socketListener,
            avatarState,
            locationsState
        ))
    }

    override fun surfaceCreated(holder: SurfaceHolder) {
        super.surfaceCreated(holder)
        val width = holder.surfaceFrame.width().toFloat()
        val height = holder.surfaceFrame.height().toFloat()

        scaleListener = ScaleListener(width, height)
        gestureListener = GestureListener() {
            // on scroll stop
            updateMapTitleState?.updateMapTitle()
        }

        scaleGestureDetector = ScaleGestureDetector(context, scaleListener!!)
        gestureDetector = GestureDetector(context, gestureListener!!)
    }


    override fun surfaceDestroyed(holder: SurfaceHolder) {
        //NativeLibrary.surfaceDestroyed()
        // похоже очень что чистит сам GLSurfaceView
        // и не нужно читстить самому open gl сущности
        super.surfaceDestroyed(holder)
    }

    override fun onDetachedFromWindow() {
        super.onDetachedFromWindow()
    }

    @SuppressLint("ClickableViewAccessibility")
    override fun onTouchEvent(event: MotionEvent): Boolean {
        when (event.action) {
            MotionEvent.ACTION_UP -> {
                //Log.d("TouchEvent", "Палец поднялся!")
                gestureListener!!.isOneFingerScaling = false
                gestureListener!!.isDragging = false
            }
        }
        scaleGestureDetector!!.onTouchEvent(event)
        gestureDetector!!.onTouchEvent(event)
        return true
    }

    fun scale(factor: Float) {
        val zoom = NativeLibrary.scale(factor)
        systemUIState?.setLight(zoom < 3.0f)
    }

    private inner class ScaleListener(
        private var width: Float,
        private var height: Float
    ) : SimpleOnScaleGestureListener() {
        override fun onScale(detector: ScaleGestureDetector): Boolean {
            scale(detector.scaleFactor)
            return true
        }

        override fun onScaleBegin(detector: ScaleGestureDetector): Boolean {
            return true
        }
    }

    private inner class GestureListener(private val onScrollStop: () -> Unit) : SimpleOnGestureListener() {
        var isOneFingerScaling = false
        var isDragging = false
        private val handler = Handler(Looper.getMainLooper())
        private val scrollStopRunnable = Runnable { onScrollStop() }

        override fun onScroll(
            e1: MotionEvent?,
            e2: MotionEvent,
            distanceX: Float,
            distanceY: Float
        ): Boolean {
            // Отменяем предыдущий таймер
            handler.removeCallbacks(scrollStopRunnable)

            // Запускаем таймер, если скролл не продолжается через 200 мс, вызываем onScrollStop()
            handler.postDelayed(scrollStopRunnable, 200)

            val x = e2.x
            val y = e2.y
            val wb = width - width / 6.0f
            val hb = height / 3.0f
            val scaleDragging = x >= wb && y >= hb
            if ( (scaleDragging || isOneFingerScaling) && !isDragging) {
                // это приближение и отдаление если правую нижниюю часть экарана трогать пальцем
                scale(1.0f + distanceY / 450.0f)
                isOneFingerScaling = true
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
            NativeLibrary.onDown()
            return true
        }

        override fun onContextClick(event: MotionEvent): Boolean {
            when (event.action) {
                MotionEvent.ACTION_UP -> {
                    isOneFingerScaling = false
                    isDragging = false
                }
            }
            return super.onContextClick(event)
        }

        override fun onSingleTapConfirmed(e: MotionEvent): Boolean {
            val screenX = e.x
            val screenY = e.y
            val selectedMarker = NativeLibrary.confirmedClick(screenX, screenY, meAvatarState?.getMeId()?: 0L)
            selectedState?.selectedMarker?.value = selectedMarker.selectedMarker
            return super.onSingleTapConfirmed(e)
        }
    }
}