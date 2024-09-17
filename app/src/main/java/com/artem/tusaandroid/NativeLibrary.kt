package com.artem.tusaandroid

import android.content.res.AssetManager

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
    external fun addMarker(key: String, latitude: Float, longitude: Float, avatarBuffer: ByteArray)
    external fun updateMarkerGeo(key: String, latitude: Float, longitude: Float)
    external fun removeMarker(key: String)
    external fun updateMarkerAvatar(key: String, avatarBuffer: ByteArray)
    external fun handleMarker(key: String, latitude: Float, longitude: Float, avatarBuffer: ByteArray)
}