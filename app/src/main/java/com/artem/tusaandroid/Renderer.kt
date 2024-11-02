package com.artem.tusaandroid

import android.content.res.AssetManager
import android.opengl.GLSurfaceView
import com.artem.tusaandroid.app.MeAvatarState
import com.artem.tusaandroid.app.avatar.AvatarState
import com.artem.tusaandroid.location.LastLocationState
import com.artem.tusaandroid.location.LocationDto
import com.artem.tusaandroid.socket.EventListener
import com.artem.tusaandroid.socket.SocketListener
import java.util.concurrent.Executors
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class Renderer(
    private val _assetManager: AssetManager,
    private val meAvatarState: MeAvatarState,
    private val lastLocationState: LastLocationState,
    private val socketListener: SocketListener,
    private val avatarState: AvatarState
) : GLSurfaceView.Renderer {
    private var defaultAvatar: ByteArray? = null
    private var artemAvatar: ByteArray? = null
    private var grishaAvatar: ByteArray? = null
    private val meAvatarKey = "me"
    private var locations: List<LocationDto> = listOf()
    private val executor = Executors.newSingleThreadExecutor()

    init {
        _assetManager.open("images/artem.png").use { asset ->
            artemAvatar = asset.readBytes()
        }
        _assetManager.open("images/grisha.jpeg").use { asset ->
            grishaAvatar = asset.readBytes()
        }
        _assetManager.open("images/user.png").use { asset ->
            defaultAvatar = asset.readBytes()
        }

        socketListener.getReceiveMessage().locationsBus.addListener(object: EventListener<List<LocationDto>> {
            override fun onEvent(event: List<LocationDto>) {
                locations = event
            }
        })

        // периодически загружаем геопозиции друзей
        executor.execute {
            Thread.sleep(1000)
            while (true) {
                socketListener.getSendMessage()?.locations()
                Thread.sleep(10000)
            }
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

        // добавляем маркера друзей
        // обновляем геопозицию маркеров друзей
        for (location in locations) {
            val key = location.ownerId.toString()
            val existMarker = NativeLibrary.existMarker(key)
            if (!existMarker) {
                val useImage = avatarState.getAvatarBytes(location.ownerId)
                if (useImage != null) {
                    NativeLibrary.addMarker(key, location.latitude, location.longitude, useImage)
                }
            } else {
                NativeLibrary.updateMarkerGeo(key, location.latitude, location.longitude)
            }
        }

        // Рендрим любимейшую мою планету
        NativeLibrary.render()
    }
}