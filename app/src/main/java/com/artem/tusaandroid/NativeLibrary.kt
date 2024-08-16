package com.artem.tusaandroid

import android.content.res.AssetManager

object NativeLibrary {
    init {
        System.loadLibrary("tusaandroid")
    }

    external fun render()
    external fun onSurfaceChanged(width: Int, height: Int)
    external fun surfaceCreated(assetManager: AssetManager?)
    external fun noOpenGlContextInit(assetManager: AssetManager?, appStartScaleFactor: Float)
    external fun setupCacheDirAbsolutePath(path: String?)
    external fun drag(dx: Float, dy: Float)
    external fun scale(scaleFactor: Float)
    external fun doubleTap()
    external fun onStop()

    external fun onDown()
}