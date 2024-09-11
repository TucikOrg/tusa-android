package com.artem.tusaandroid.location

import android.content.Intent
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.LocationOn
import androidx.compose.material3.ElevatedButton
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import com.google.accompanist.permissions.ExperimentalPermissionsApi
import com.google.accompanist.permissions.isGranted
import com.google.accompanist.permissions.rememberPermissionState
import kotlinx.coroutines.launch

@Preview
@Composable
fun LocationSetupCardPreview() {
    LocationSetupCard(model = PreviewLocationSetupCardViewModel())
}

@OptIn(ExperimentalPermissionsApi::class)
@Composable
fun LocationSetupCard(model: LocationSetupCardViewModel) {
    val coroutineScope = rememberCoroutineScope()
    val permissionState = rememberPermissionState(permission = android.Manifest.permission.ACCESS_FINE_LOCATION)
    if (permissionState.status.isGranted) {

        val context = LocalContext.current
        ElevatedButton(
            modifier = Modifier.fillMaxWidth(),
            shape = RoundedCornerShape(16.dp),
            onClick = {
                if (model.locationServiceStarted.value == false) {
                    val startIntent = Intent(context, LocationForegroundService::class.java).apply {
                        action = LocationForegroundService.ACTION_START
                    }
                    context.startService(startIntent)
                } else {
                    val stopIntent = Intent(context, LocationForegroundService::class.java).apply {
                        action = LocationForegroundService.ACTION_STOP
                    }
                    context.startService(stopIntent)
                }

                model.locationServiceStarted.value = model.locationServiceStarted.value?.not()
            }
        ) {
            if (model.locationServiceStarted.value == true) {
                Text("Спрятать меня на карте")
            } else {
                Text("Показывать меня на карте")
            }
        }

        return
    }

    ElevatedButton(
        modifier = Modifier.fillMaxWidth(),
        shape = RoundedCornerShape(16.dp),
        onClick = {
            coroutineScope.launch {
                permissionState.launchPermissionRequest()
            }
        }
    ) {
        Text(
            text = "Дать разрешение на геолокацию",
            fontWeight = FontWeight.Bold,
            fontSize = MaterialTheme.typography.headlineSmall.fontSize
        )
        Spacer(modifier = Modifier.width(8.dp))
        Icon(
            imageVector = Icons.Default.LocationOn,
            contentDescription = "search me on map",
            modifier = Modifier.size(30.dp)
        )
    }
}