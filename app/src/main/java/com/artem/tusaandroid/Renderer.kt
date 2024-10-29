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
    private var artemAvatar: ByteArray? = null
    private var grishaAvatar: ByteArray? = null
    private val meAvatarKey = "me"

    init {
        _assetManager.open("images/artem.png").use { asset ->
            artemAvatar = asset.readBytes()
        }
        _assetManager.open("images/grisha.jpeg").use { asset ->
            grishaAvatar = asset.readBytes()
        }
        _assetManager.open("images/person.png").use { asset ->
            defaultAvatar = asset.readBytes()
        }
    }

    override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
        NativeLibrary.surfaceCreated(_assetManager)
        // NativeLibrary.addMarker("artem", 55.7558f, 37.6176f, artemAvatar!!)
    }

    override fun onSurfaceChanged(gl: GL10, width: Int, height: Int) {
        NativeLibrary.onSurfaceChanged(width, height)
    }

    override fun onDrawFrame(gl: GL10) {
        // Update me avatar marker
        // if avatar changed and location exists
        // Добавить маркер или сменить аватарку маркера пользователя
        if (meAvatarState.getNeedUpdateInRenderFlag() && lastLocationState.lastLocationExists()) {
            var useMeAvatar = meAvatarState.getAvatarBytes()
            if (useMeAvatar == null) {
                useMeAvatar = defaultAvatar
            }

            val latitude = lastLocationState.getLastLatitude()
            val longitude = lastLocationState.getLastLongitude()
            val existMarker = NativeLibrary.existMarker(meAvatarKey)
            if (!existMarker) {
                NativeLibrary.addMarker(meAvatarKey, latitude, longitude, useMeAvatar!!)
            } else {
                NativeLibrary.updateMarkerAvatar(meAvatarKey, useMeAvatar!!)
            }
            meAvatarState.rendererUpdated()
        }

        // Update me avatar location if location changed
        // Изменить местоположение маркера пользователя
        if (lastLocationState.lastLocationExists() && lastLocationState.getNeedUpdateLastLocationInRenderer()) {
            val latitude = lastLocationState.getLastLatitude()
            val longitude = lastLocationState.getLastLongitude()
            NativeLibrary.updateMarkerGeo(meAvatarKey, latitude, longitude)
            lastLocationState.rendererUpdatedLastLocation()
        }

        // Нужно спрятать маркер пользователя
        // например когда он отключил показ себя на карте
        if (meAvatarState.getHideMeFlag()) {
            NativeLibrary.removeMarker(meAvatarKey)
            meAvatarState.iAmHidden()
        }

        // Рендрим любимейшую мою планету
        NativeLibrary.render()
    }
}