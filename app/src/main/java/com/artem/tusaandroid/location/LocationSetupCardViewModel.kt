package com.artem.tusaandroid.location

import android.content.Context
import android.content.Intent
import android.location.LocationManager
import androidx.compose.runtime.mutableStateOf
import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.app.MeAvatarState
import com.artem.tusaandroid.app.dialog.AppDialogState
import com.artem.tusaandroid.notification.NotificationsService
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
open class LocationSetupCardViewModel @Inject constructor(
    private val lastLocationState: LastLocationState?,
    private val meAvatarState: MeAvatarState?,
    private val notificationsService: NotificationsService?,
    private val appDialogState: AppDialogState?
): ViewModel() {
    var locationServiceStarted = lastLocationState?.getLocationForegroundServiceStartedState() ?: mutableStateOf(false)

    fun showGpsDisabledAlert() {
        appDialogState?.showGpsDisabledAlert(this)
    }

    fun closeGPSDisabledAlert() {
        appDialogState?.closeGpsDisabledAlert()
    }

    fun determineInitState(context: Context) {
        val locationManager = context.getSystemService(Context.LOCATION_SERVICE) as LocationManager
        val gpsEnabled = locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER)
        locationServiceStarted.value = lastLocationState?.getLocationForegroundServiceStarted() == true && gpsEnabled
    }

    fun switchMyLocationState(context: Context) {
        val locationManager = context.getSystemService(Context.LOCATION_SERVICE) as LocationManager
        if (locationServiceStarted.value == false) {
            val gpsEnabled = locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER)
            if (!gpsEnabled) {
                showGpsDisabledAlert()
                return
            }
            if (notificationsService?.check(context) == true) {
                return
            }

            val startIntent = Intent(context, LocationForegroundService::class.java).apply {
                action = LocationForegroundService.ACTION_START
                flags = Intent.FLAG_ACTIVITY_SINGLE_TOP or Intent.FLAG_ACTIVITY_CLEAR_TOP
            }
            context.startForegroundService(startIntent)
            locationServiceStarted.value = true
            meAvatarState?.updateMeMarkerInRender()
        } else {
            val stopIntent = Intent(context, LocationForegroundService::class.java).apply {
                action = LocationForegroundService.ACTION_STOP
                flags = Intent.FLAG_ACTIVITY_SINGLE_TOP or Intent.FLAG_ACTIVITY_CLEAR_TOP
            }
            context.stopService(stopIntent)
            locationServiceStarted.value = false
            meAvatarState?.hideMe()
        }
    }
}