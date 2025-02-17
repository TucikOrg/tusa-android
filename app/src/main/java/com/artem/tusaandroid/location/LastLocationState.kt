package com.artem.tusaandroid.location

import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import com.artem.tusaandroid.StateHasSharedPreferences
import java.time.LocalDateTime
import java.time.ZoneOffset

// это класс где храниться местоположение пользователя
class LastLocationState: StateHasSharedPreferences() {

    private var lastLocationExists: MutableState<Boolean> = mutableStateOf(false)
    private var latitude: Float? = null
    private var longitude: Float? = null
    private var lastLocationUpdateTime: LocalDateTime? = null
    private var needUpdateLastLocationInRenderer = false
    private var locationForegroundServiceStarted: MutableState<Boolean> = mutableStateOf(false)

    fun getLastLocationExists(): MutableState<Boolean> {
        return lastLocationExists
    }

    fun lastLocationExists(): Boolean {
        return lastLocationExists.value
    }

    override fun onLoad(userId: Long) {
        locationForegroundServiceStarted.value = sharedPreferences.getBoolean("locationForegroundServiceStarted", false)
    }

    fun saveLocationForegroundServiceStarted(value: Boolean) {
        this.locationForegroundServiceStarted.value = value
        sharedPreferences.edit().putBoolean("locationForegroundServiceStarted", value).apply()
    }

    fun getLocationForegroundServiceStartedState(): MutableState<Boolean> {
        return locationForegroundServiceStarted
    }

    fun getLocationForegroundServiceStarted(): Boolean {
        return locationForegroundServiceStarted.value
    }

    fun getNeedUpdateLastLocationInRenderer(): Boolean {
        return needUpdateLastLocationInRenderer
    }

    private fun lastLocationExistsInner(): Boolean {
        return latitude != null && longitude != null && lastLocationUpdateTime != null
    }

    fun getLastLocationUpdateTime(): LocalDateTime {
        return lastLocationUpdateTime!!
    }

    fun getLastLatitude(): Float {
        return latitude!!
    }

    fun getLastLongitude(): Float {
        return longitude!!
    }

    fun setLastLatitude(value: Float) {
        latitude = value
        lastLocationUpdateTime = LocalDateTime.now(ZoneOffset.UTC)
        needUpdateLastLocationInRenderer = true
        lastLocationExists.value = lastLocationExistsInner()
    }

    fun setLastLongitude(value: Float) {
        longitude = value
        lastLocationUpdateTime = LocalDateTime.now(ZoneOffset.UTC)
        needUpdateLastLocationInRenderer = true
        lastLocationExists.value = lastLocationExistsInner()
    }

    fun rendererUpdatedLastLocation() {
        needUpdateLastLocationInRenderer = false
    }
}