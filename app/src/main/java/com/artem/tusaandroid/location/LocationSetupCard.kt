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
import androidx.compose.foundation.layout.Column
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
    val permissionStateFitness = rememberPermissionState(permission = android.Manifest.permission.ACTIVITY_RECOGNITION)

    if (permissionState.status.isGranted && permissionStateFitness.status.isGranted) {
        LaunchedEffect(Unit) {
            model.determineInitState(context)
        }

        ElevatedButton(
            modifier = Modifier.fillMaxWidth(),
            shape = RoundedCornerShape(16.dp),
            onClick = {
                model.switchMyLocationState(context)
            }
        ) {
            if (model.locationServiceStarted.value == true) {
                HideMeInner()
            } else {
                ShowMeInner()
            }
        }

        return
    }

    Column {
        ElevatedButton(
            modifier = Modifier.fillMaxWidth(),
            shape = RoundedCornerShape(16.dp),
            onClick = {
                model.switchMyLocationState(context)
            },
            enabled = model.locationServiceStarted.value == true
        ) {
            if (model.locationServiceStarted.value == true) {
                HideMeInner()
            } else {
                ShowMeInner()
            }
        }

        if (permissionState.status.isGranted == false) {
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
        } else if (permissionStateFitness.status.isGranted == false) {
            ElevatedButton(
                modifier = Modifier.fillMaxWidth(),
                shape = RoundedCornerShape(16.dp),
                onClick = {
                    coroutineScope.launch {
                        permissionStateFitness.launchPermissionRequest()
                    }
                }
            ) {
                Text(
                    text = "Дать разрешение на доступ к шагам",
                    fontWeight = FontWeight.Bold,
                    fontSize = MaterialTheme.typography.bodyMedium.fontSize
                )
                Spacer(modifier = Modifier.width(8.dp))
                Icon(
                    painter = painterResource(id = R.drawable.sprint),
                    contentDescription = "Доступ к активности",
                    modifier = Modifier.size(30.dp)
                )
            }
        }
    }
}

@Composable
fun HideMeInner() {
    Text("Спрятать меня на карте")
    Spacer(modifier = Modifier.width(8.dp))
    Icon(
        painter = painterResource(id = R.drawable.location_off),
        contentDescription = "Искать меня на карте",
        modifier = Modifier.size(30.dp)
    )
}

@Composable
fun ShowMeInner() {
    Text("Показывать меня на карте")
    Spacer(modifier = Modifier.width(8.dp))
    Icon(
        painter = painterResource(id = R.drawable.globe_marker),
        contentDescription = "Искать меня на карте",
        modifier = Modifier.size(30.dp)
    )
}