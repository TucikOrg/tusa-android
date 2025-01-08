package com.artem.tusaandroid

import android.content.res.AssetManager
import com.artem.tusaandroid.location.CameraPos

object NativeLibrary {
    init {
        System.loadLibrary("tusaandroid")
    }

    external fun create()
    external fun cleanup()

    external fun render()
    external fun onSurfaceChanged(width: Int, height: Int)
    external fun surfaceCreated(assetManager: AssetManager?)
    external fun surfaceDestroyed()
    external fun noOpenGlContextInit(assetManager: AssetManager?, requestTile: RequestTile)
    external fun drag(dx: Float, dy: Float)
    external fun scale(scaleFactor: Float): Float
    external fun doubleTap()
    external fun onDown()
    external fun setCameraPos(latitude: Float, longitude: Float, zoom: Float)
    external fun getCameraPos(): CameraPos
    external fun confirmedClick(x: Float, y: Float, ignore: Long): MapClickResult
    external fun deselectSelectedMarker()

    // Markers
    external fun removeMarker(key: Long)
    external fun updateMarkerAvatar(key: Long, avatarBuffer: ByteArray)
    external fun updateMarkerGeo(key: Long, latitude: Float, longitude: Float)
    external fun addMarker(key: Long, latitude: Float, longitude: Float, avatarBuffer: ByteArray)
    external fun existMarker(key: Long): Boolean
}