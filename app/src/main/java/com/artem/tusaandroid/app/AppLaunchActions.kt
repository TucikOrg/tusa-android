package com.artem.tusaandroid.app

import android.Manifest
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.location.LocationManager
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.ui.platform.LocalContext
import androidx.core.app.ActivityCompat
import com.artem.tusaandroid.TucikViewModel
import com.artem.tusaandroid.isPreview
import com.artem.tusaandroid.location.LocationForegroundService
import com.artem.tusaandroid.notification.NotificationsEnabledCheck
import com.artem.tusaandroid.notification.NotificationsEnabledCheckViewModelPreview
import com.google.accompanist.permissions.ExperimentalPermissionsApi

@OptIn(ExperimentalPermissionsApi::class)
@Composable
fun AppLaunchActions(model: AppLaunchActionsViewModel) {
    // Заранее производим загрузки из интернета, чтобы не ждать их во время работы
    val context = LocalContext.current
    NotificationsEnabledCheck(
        model = TucikViewModel(model.isPreview(),
        previewModel = NotificationsEnabledCheckViewModelPreview())
    )

    LaunchedEffect(Unit) {
        if (ActivityCompat.checkSelfPermission(
                context,
                Manifest.permission.ACCESS_FINE_LOCATION
            ) == PackageManager.PERMISSION_GRANTED
        ) {
            // Если сервис был запущен и разрешение на геолокацию есть и он был запущен, то запускаем сервис
            // это еще так же значит что пользователь желает отображать себя на карте
            // + gps включен
            val locationManager = context.getSystemService(Context.LOCATION_SERVICE) as LocationManager
            val gpsEnabled = locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER)
            if (model.getLocationForegroundServiceStarted() == true && gpsEnabled) {
                val startIntent = Intent(context, LocationForegroundService::class.java).apply {
                    action = LocationForegroundService.ACTION_START
                }
                context.startService(startIntent)
                model.createMeMarker() // маркер юзера создаем
            }
        } else {
            // Если сервис был запущен, но разрешение на геолокацию отозвали, то останавливаем сервис
            model.setLocationForegroundServiceStarted(false)
        }

        model.connectToSocket()
    }
}