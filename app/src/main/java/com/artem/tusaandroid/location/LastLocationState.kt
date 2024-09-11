package com.artem.tusaandroid.location

import com.artem.tusaandroid.StateHasSharedPreferences
import java.time.LocalDateTime

class LastLocationState: StateHasSharedPreferences() {

    private var latitude: Float? = null
    private var longitude: Float? = null
    private var lastLocationUpdateTime: LocalDateTime? = null
    private var needUpdateLastLocationInRenderer = false
    private var locationForegroundServiceStarted = false

    override fun onLoad() {
        locationForegroundServiceStarted = sharedPreferences.getBoolean("locationForegroundServiceStarted", false)
    }

    fun saveLocationForegroundServiceStarted(value: Boolean) {
        this.locationForegroundServiceStarted = value
        sharedPreferences.edit().putBoolean("locationForegroundServiceStarted", value).apply()
    }

    fun getLocationForegroundServiceStarted(): Boolean {
        return locationForegroundServiceStarted
    }

    fun getNeedUpdateLastLocationInRenderer(): Boolean {
        return needUpdateLastLocationInRenderer
    }

    fun lastLocationExists(): Boolean {
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
        lastLocationUpdateTime = LocalDateTime.now()
        needUpdateLastLocationInRenderer = true
    }

    fun setLastLongitude(value: Float) {
        longitude = value
        lastLocationUpdateTime = LocalDateTime.now()
        needUpdateLastLocationInRenderer = true
    }

    fun rendererUpdatedLastLocation() {
        needUpdateLastLocationInRenderer = false
    }



}