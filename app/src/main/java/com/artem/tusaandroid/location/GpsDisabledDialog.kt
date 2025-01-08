package com.artem.tusaandroid.location

import android.content.Context
import android.content.Intent
import android.location.LocationManager
import android.provider.Settings
import androidx.compose.material3.AlertDialog
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.ui.platform.LocalContext

@Composable
fun GpsDisabledDialog(model: LocationSetupCardViewModel) {
    val context = LocalContext.current
    val locationManager = context.getSystemService(Context.LOCATION_SERVICE) as LocationManager
    AlertDialog(
        title = {
            Text("GPS выключен")
        },
        text = {
            Text("Включите его в верхней панели устройства или мы можем открыть меню с этой настройкой.")
        },
        dismissButton = {
            TextButton(
                onClick = {
                    model.gpsDisabledAlert.value = false
                }
            ) {
                Text("Закрыть")
            }
        },
        confirmButton = {
            TextButton(
                onClick = {
                    val gpsEnabled = locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER)
                    if (!gpsEnabled) {
                        val intent = Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS).apply {
                            flags = Intent.FLAG_ACTIVITY_SINGLE_TOP or Intent.FLAG_ACTIVITY_CLEAR_TOP
                        }
                        context.startActivity(intent)
                    }
                    model.gpsDisabledAlert.value = false
                }
            ) {
                Text("Включить GPS")
            }
        },
        onDismissRequest = {
            model.gpsDisabledAlert.value = false
        },
    )
}