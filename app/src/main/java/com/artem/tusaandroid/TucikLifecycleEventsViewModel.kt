package com.artem.tusaandroid

import android.Manifest
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.location.LocationManager
import androidx.core.app.ActivityCompat
import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.app.dialog.AppDialogState
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.location.LastLocationState
import com.artem.tusaandroid.location.LocationForegroundService
import com.artem.tusaandroid.socket.SocketListener
import com.google.android.gms.common.ConnectionResult
import com.google.android.gms.common.GoogleApiAvailability
import com.google.android.gms.fitness.LocalRecordingClient
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
open class TucikLifecycleEventsViewModel @Inject constructor(
    val socketListener: SocketListener,
    private val appDialogState: AppDialogState,
    private val lastLocationState: LastLocationState,
    private val profileState: ProfileState
) : ViewModel() {
    fun makeOnStart(context: Context) {
        // нету нужного гугл сервиса
        val hasMinPlayServices = GoogleApiAvailability.getInstance()
            .isGooglePlayServicesAvailable(context, LocalRecordingClient.LOCAL_RECORDING_CLIENT_MIN_VERSION_CODE)
        if(hasMinPlayServices != ConnectionResult.SUCCESS) {
            // Prompt user to update their device's Google Play services app and return
            appDialogState.open("Google Play Servcies", "Для коректной работы приложения необходимо обновить Google Play Services")
        }

        val locationPermissionGranted = ActivityCompat.checkSelfPermission(
            context,
            android.Manifest.permission.ACCESS_FINE_LOCATION
        ) == PackageManager.PERMISSION_GRANTED

        val activityRecognitionPermissionGranted = ActivityCompat.checkSelfPermission(
            context,
            Manifest.permission.ACTIVITY_RECOGNITION
        ) == PackageManager.PERMISSION_GRANTED

        if ( locationPermissionGranted && activityRecognitionPermissionGranted ) {
            // Если сервис был запущен и разрешение на геолокацию есть и он был запущен, то запускаем сервис
            // это еще так же значит что пользователь желает отображать себя на карте
            // + gps включен
            val locationManager = context.getSystemService(Context.LOCATION_SERVICE) as LocationManager
            val gpsEnabled = locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER)
            if (lastLocationState.getLocationForegroundServiceStarted() == true && gpsEnabled) {
                val startIntent = Intent(context, LocationForegroundService::class.java).apply {
                    action = LocationForegroundService.ACTION_START
                    flags = Intent.FLAG_ACTIVITY_SINGLE_TOP or Intent.FLAG_ACTIVITY_CLEAR_TOP
                }
                context.startForegroundService(startIntent)
            }
        } else {
            // Если сервис был запущен, но разрешение на геолокацию или фитнесс отозвали, то останавливаем сервис
            lastLocationState.saveLocationForegroundServiceStarted(false)
        }

        if (profileState.getIsAuthenticated() == true) {
            socketListener.connect()
        }
    }
}