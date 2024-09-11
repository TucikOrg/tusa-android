package com.artem.tusaandroid

import android.content.res.AssetManager
import android.opengl.GLSurfaceView
import com.artem.tusaandroid.app.MeAvatarState
import com.artem.tusaandroid.location.LastLocationState
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class Renderer(
    private val _assetManager: AssetManager,
    private val meAvatarState: MeAvatarState,
    private val lastLocationState: LastLocationState
) : GLSurfaceView.Renderer {
    private var defaultAvatar: ByteArray? = null

    init {
        _assetManager.open("images/artem.png").use { asset ->
            defaultAvatar = asset.readBytes()
        }
    }

    override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
        NativeLibrary.surfaceCreated(_assetManager)
    }

    override fun onSurfaceChanged(gl: GL10, width: Int, height: Int) {
        NativeLibrary.onSurfaceChanged(width, height)
    }

    override fun onDrawFrame(gl: GL10) {
        // Update me avatar marker
        // if avatar changed and location exists
        if (meAvatarState.getNeedUpdateInRenderFlag() && lastLocationState.lastLocationExists()) {
            var useMeAvatar = meAvatarState.getAvatarBytes()
            if (useMeAvatar == null) {
                useMeAvatar = defaultAvatar
            }

            val latitude = lastLocationState.getLastLatitude()
            val longitude = lastLocationState.getLastLongitude()
            NativeLibrary.handleMarker("me", latitude, longitude, useMeAvatar!!)
            meAvatarState.rendererUpdated()
        }

        // Update me avatar location if location changed
        if (lastLocationState.lastLocationExists() && lastLocationState.getNeedUpdateLastLocationInRenderer()) {
            val latitude = lastLocationState.getLastLatitude()
            val longitude = lastLocationState.getLastLongitude()
            NativeLibrary.updateMarkerGeo("me", latitude, longitude)
            lastLocationState.rendererUpdatedLastLocation()
        }

        NativeLibrary.render()
    }
}