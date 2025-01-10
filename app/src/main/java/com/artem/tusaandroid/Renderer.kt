package com.artem.tusaandroid

import android.content.res.AssetManager
import android.opengl.GLSurfaceView
import com.artem.tusaandroid.app.MeAvatarState
import com.artem.tusaandroid.app.avatar.AvatarState
import com.artem.tusaandroid.location.LastLocationState
import com.artem.tusaandroid.location.LocationsState
import com.artem.tusaandroid.socket.SocketListener
import java.util.concurrent.Executors
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class Renderer(
    private val _assetManager: AssetManager,
    private val meAvatarState: MeAvatarState?,
    private val lastLocationState: LastLocationState?,
    private val socketListener: SocketListener?,
    private val avatarState: AvatarState?,
    private val locationsState: LocationsState?
) : GLSurfaceView.Renderer {
    private var defaultAvatar: ByteArray? = null

    private var meAvatarKey = meAvatarState?.getMeId()?: 0L
    private val executor = Executors.newSingleThreadExecutor()

    init {
        _assetManager.open("images/default_user.png").use { asset ->
            defaultAvatar = asset.readBytes()
        }

        // периодически загружаем геопозиции друзей
        executor.execute {
            Thread.sleep(1000)
            while (true) {
                socketListener?.getSendMessage()?.locations()
                Thread.sleep(5000)
            }
        }
    }

    override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
        NativeLibrary.surfaceCreated(_assetManager)
    }

    override fun onSurfaceChanged(gl: GL10, width: Int, height: Int) {
        NativeLibrary.onSurfaceChanged(width, height)
    }

    override fun onDrawFrame(gl: GL10) {
        // обновляет и работает только с моим аватоаром
        // Добавить маркер или сменить аватарку маркера пользователя
        if (meAvatarState?.getNeedUpdateInRenderFlag() == true && lastLocationState?.lastLocationExists() == true) {
            var useMeAvatar = meAvatarState.getAvatarBytes()
            if (useMeAvatar == null) {
                useMeAvatar = defaultAvatar
            }

            val latitude = lastLocationState.getLastLatitude()
            val longitude = lastLocationState.getLastLongitude()
            // если маркер есть уже то просто обновит аватар и сделает его видимым
            // если нету то создаст его
            NativeLibrary.addMarker(meAvatarKey, latitude, longitude, useMeAvatar!!)
            meAvatarState.rendererUpdated()
        }

        // Обновить мою локацию если она изменилась
        // Изменить местоположение маркера пользователя
        if (lastLocationState?.lastLocationExists() == true && lastLocationState.getNeedUpdateLastLocationInRenderer()) {
            val latitude = lastLocationState.getLastLatitude()
            val longitude = lastLocationState.getLastLongitude()
            NativeLibrary.updateMarkerGeo(meAvatarKey, latitude, longitude)
            lastLocationState.rendererUpdatedLastLocation()
        }

        // Нужно спрятать маркер пользователя
        // например когда он отключил показ себя на карте
        if (meAvatarState?.getHideMeFlag() == true) {
            NativeLibrary.removeMarker(meAvatarKey)
            meAvatarState.iAmHidden()
        }

        // добавляем маркера друзей
        // обновляем геопозицию маркеров друзей
        val currentLocations = locationsState?.currentLocations ?: listOf()
        for (location in currentLocations) {
            val key = location.ownerId
            val existMarker = NativeLibrary.existMarker(key)
            if (!existMarker) {
                val useImage = avatarState?.getAvatarBytes(location.ownerId)
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