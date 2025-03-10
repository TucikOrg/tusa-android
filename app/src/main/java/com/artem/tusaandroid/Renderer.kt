package com.artem.tusaandroid

import android.content.res.AssetManager
import android.opengl.EGL14
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
    private var meAvatarKey = meAvatarState?.getMeId()?: 0L
    private var secondaryGLThread: SecondaryGLThread? = null

    fun destroySecondaryGLThread() {
        secondaryGLThread?.destroy()
    }

    override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
        NativeLibrary.surfaceCreated(_assetManager)

        val sharedEglContext = EGL14.eglGetCurrentContext()
        secondaryGLThread = SecondaryGLThread(sharedEglContext, 8192, 8192)
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
                useMeAvatar = avatarState?.getDefaultAvatar()?: byteArrayOf()
            }

            val latitude = lastLocationState.getLastLatitude()
            val longitude = lastLocationState.getLastLongitude()
            // если маркер есть уже то просто обновит аватар и сделает его видимым
            // если нету то создаст его
            NativeLibrary.addMarker(meAvatarKey, latitude, longitude, useMeAvatar, true)
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
        val friendsLocations = locationsState?.friendLocations ?: listOf()
        for (location in friendsLocations) {
            if (location.updateMarkerFlag == false) continue

            val key = location.ownerId
            val useImage = avatarState?.getAvatarBytes(location.ownerId)?: avatarState?.getDefaultAvatar()
            if (useImage != null) {
                NativeLibrary.addMarker(key, location.latitude, location.longitude, useImage, location.updateAvatar)
                location.updateAvatar = false
            }
            location.updateMarkerFlag = false
        }

        // Рендрим любимейшую мою планету
        NativeLibrary.render(BuildConfig.MAP_DEBUG)
    }
}