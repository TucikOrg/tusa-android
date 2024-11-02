package com.artem.tusaandroid.app

import android.Manifest
import android.content.Intent
import android.content.pm.PackageManager
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.ui.platform.LocalContext
import androidx.core.app.ActivityCompat
import com.artem.tusaandroid.location.LocationForegroundService

@Composable
fun AppLaunchActions(model: AppLaunchActionsViewModel) {
    // Заранее производим загрузки из интернета, чтобы не ждать их во время работы
    val context = LocalContext.current
    LaunchedEffect(Unit) {
        if (ActivityCompat.checkSelfPermission(
                context,
                Manifest.permission.ACCESS_FINE_LOCATION
            ) == PackageManager.PERMISSION_GRANTED
        ) {
            // Если сервис был запущен и разрешение на геолокацию есть и он был запущен, то запускаем сервис
            if (model.getLocationForegroundServiceStarted() == true) {
                val startIntent = Intent(context, LocationForegroundService::class.java).apply {
                    action = LocationForegroundService.ACTION_START
                }
                context.startService(startIntent)
            }
        } else {
            // Если сервис был запущен, но разрешение на геолокацию отозвали, то останавливаем сервис
            model.setLocationForegroundServiceStarted(false)
        }

        model.connectToSocket()
    }
}