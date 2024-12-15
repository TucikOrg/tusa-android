package com.artem.tusaandroid

import android.content.res.AssetManager
import com.artem.tusaandroid.location.CameraPos

object NativeLibrary {
    init {
        System.loadLibrary("tusaandroid")
    }

    external fun render()
    external fun onSurfaceChanged(width: Int, height: Int)
    external fun surfaceCreated(assetManager: AssetManager?)
    external fun noOpenGlContextInit(assetManager: AssetManager?, requestTile: RequestTile)
    external fun drag(dx: Float, dy: Float)
    external fun scale(scaleFactor: Float)
    external fun doubleTap()
    external fun onStop()
    external fun onDown()
    external fun setCameraPos(latitude: Float, longitude: Float, zoom: Float)
    external fun getCameraPos(): CameraPos

    // Markers
    external fun removeMarker(key: Long)
    external fun updateMarkerAvatar(key: Long, avatarBuffer: ByteArray)
    external fun updateMarkerGeo(key: Long, latitude: Float, longitude: Float)
    external fun addMarker(key: Long, latitude: Float, longitude: Float, avatarBuffer: ByteArray)
    external fun existMarker(key: Long): Boolean
}