package com.artem.tusaandroid.location

import android.Manifest
import android.annotation.SuppressLint
import android.content.pm.PackageManager
import androidx.core.app.ActivityCompat
import com.artem.tusaandroid.app.logs.AppLogsState
import com.artem.tusaandroid.await
import com.artem.tusaandroid.dto.AddLocationDto
import com.artem.tusaandroid.requests.CustomTucikEndpoints
import com.google.android.gms.location.CurrentLocationRequest
import com.google.android.gms.location.FusedLocationProviderClient
import com.google.android.gms.location.LocationServices
import com.google.android.gms.location.Priority
import com.google.android.gms.tasks.CancellationTokenSource
import com.squareup.moshi.Moshi
import com.squareup.moshi.adapter
import okhttp3.MediaType.Companion.toMediaTypeOrNull
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody.Companion.toRequestBody
import kotlin.time.Duration

class LocationUpdatingState(
    private var fusedLocationClient: FusedLocationProviderClient,
    private var lastLocationState: LastLocationState,
    private val customTucikEndpoints: CustomTucikEndpoints,
    private var moshi: Moshi,
    private var okHttpClient: OkHttpClient,
    private var appLogsState: AppLogsState
) {
    private val highUpdateTime = "1m"
    private val mediumUpdateTime = "3m"
    private val lowUpdateTime = "4m"

    private var updateLocationInterval = mediumUpdateTime

    fun getUpdateLocationInterval(): String {
        return updateLocationInterval
    }

    @SuppressLint("MissingPermission")
    @OptIn(ExperimentalStdlibApi::class)
    suspend fun updateLocation(reason: String, force: Boolean = false) {
        val interval = Duration.parse(getUpdateLocationInterval())
        var locationRequestBuilder = CurrentLocationRequest.Builder()
            .setPriority(Priority.PRIORITY_HIGH_ACCURACY)
            .setDurationMillis(30_000)
        if (force == false) {
            locationRequestBuilder = locationRequestBuilder
                .setMaxUpdateAgeMillis(interval.inWholeMilliseconds)
            appLogsState.addRow(
                message = "Requesting location update (maxUpdateAgeMillis: ${interval.inWholeMilliseconds})",
                reason = reason
            )
        } else {
            appLogsState.addRow(
                message = "Requesting location update",
                reason = reason
            )
        }


        val locationRequest = locationRequestBuilder.build()
        val location =
            fusedLocationClient.getCurrentLocation(
                locationRequest,
                CancellationTokenSource().token
            ).await()

        appLogsState.addRow("Location update received: $location", "")

        if (location != null) {
            val latitude = location.latitude
            val longitude = location.longitude
            lastLocationState.setLastLatitude(latitude.toFloat())
            lastLocationState.setLastLongitude(longitude.toFloat())

            val request = Request.Builder()
                .url(customTucikEndpoints.makeAddLocation())
                .post(
                    moshi.adapter<AddLocationDto>().toJson(AddLocationDto(latitude.toFloat(), longitude.toFloat()))
                        .toRequestBody("application/json".toMediaTypeOrNull())
                )
                .build()

            try {
                okHttpClient.newCall(request).execute()
                appLogsState.addRow("My location has delivered to the server", "")
            } catch (exception: Exception) {
                appLogsState.addRow("My location has not delivered to the server due network error", "")
                exception.printStackTrace()
            }
        }
    }
}