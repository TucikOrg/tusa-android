package com.artem.tusaandroid.location

import android.Manifest
import android.content.Intent
import android.content.pm.PackageManager
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.ui.platform.LocalContext
import androidx.core.app.ActivityCompat

@Composable
fun StartForegroundIfHaveLocationPermission(model: StartForegroundIfHaveLocationPermissionViewModel) {
    val context = LocalContext.current
    LaunchedEffect(Unit) {
        if (ActivityCompat.checkSelfPermission(
                context,
                Manifest.permission.ACCESS_FINE_LOCATION
            ) == PackageManager.PERMISSION_GRANTED
        ) {
            if (model.getLocationForegroundServiceStarted() == true) {
                val startIntent = Intent(context, LocationForegroundService::class.java).apply {
                    action = LocationForegroundService.ACTION_START
                }
                context.startService(startIntent)
            }
        }
    }
}