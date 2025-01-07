package com.artem.tusaandroid.location

import com.artem.tusaandroid.R
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
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
import android.location.LocationManager
import android.provider.Settings
import androidx.compose.material3.AlertDialog
import androidx.compose.material3.TextButton
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.ui.res.painterResource

@Preview
@Composable
fun LocationSetupCardPreview() {
    LocationSetupCard(model = PreviewLocationSetupCardViewModel())
}

@OptIn(ExperimentalPermissionsApi::class)
@Composable
fun LocationSetupCard(model: LocationSetupCardViewModel) {
    val context = LocalContext.current
    val coroutineScope = rememberCoroutineScope()
    val permissionState = rememberPermissionState(permission = android.Manifest.permission.ACCESS_FINE_LOCATION)
    if (permissionState.status.isGranted) {
        LaunchedEffect(Unit) {
            model.determineInitState(context)
        }

        if (model.gpsDisabledAlert.value) {
            GpsDisabledDialog(model)
        }

        ElevatedButton(
            modifier = Modifier.fillMaxWidth(),
            shape = RoundedCornerShape(16.dp),
            onClick = {
                model.switchMyLocationState(context)
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
            fontSize = MaterialTheme.typography.bodyMedium.fontSize
        )
        Spacer(modifier = Modifier.width(8.dp))
        Icon(
            painter = painterResource(id = R.drawable.location_on),
            contentDescription = "Искать меня на карте",
            modifier = Modifier.size(30.dp)
        )
    }
}