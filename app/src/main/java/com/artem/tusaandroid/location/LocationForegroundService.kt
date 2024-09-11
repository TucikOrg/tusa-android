package com.artem.tusaandroid.location

import android.Manifest
import android.app.Notification
import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.PendingIntent
import android.app.Service
import android.content.Intent
import android.content.pm.PackageManager
import android.os.IBinder
import androidx.core.app.ActivityCompat
import androidx.core.app.NotificationCompat
import com.artem.tusaandroid.AppVariables
import com.artem.tusaandroid.MainActivity
import com.artem.tusaandroid.R
import com.artem.tusaandroid.api.LocationControllerApi
import com.artem.tusaandroid.await
import com.artem.tusaandroid.model.AddLocationDto
import com.google.android.gms.location.FusedLocationProviderClient
import com.google.android.gms.location.LocationServices
import com.google.android.gms.tasks.CancellationTokenSource
import dagger.hilt.android.AndroidEntryPoint
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.cancel
import kotlinx.coroutines.delay
import kotlinx.coroutines.isActive
import kotlinx.coroutines.launch
import org.openapitools.client.infrastructure.ClientException
import javax.inject.Inject
import kotlin.time.Duration

@AndroidEntryPoint
class LocationForegroundService: Service() {
    private lateinit var fusedLocationClient: FusedLocationProviderClient
    private val serviceScope = CoroutineScope(Dispatchers.IO)

    @Inject
    lateinit var appVariables: AppVariables
    @Inject
    lateinit var lastLocationState: LastLocationState
    @Inject
    lateinit var locationControllerApi: LocationControllerApi

    override fun onStartCommand(intent: Intent, flags: Int, startId: Int): Int {
        if (intent.action == ACTION_STOP) {
            stopSelf()
            lastLocationState.saveLocationForegroundServiceStarted(false)
            return START_NOT_STICKY
        }
        lastLocationState.saveLocationForegroundServiceStarted(true)
        fusedLocationClient = LocationServices.getFusedLocationProviderClient(applicationContext)
        createNotificationChannel()
        val notificationIntent = Intent(this, MainActivity::class.java)
        val pendingIntent = PendingIntent.getActivity(this, 0, notificationIntent, PendingIntent.FLAG_IMMUTABLE)

        // Intent to stop the service
        val stopIntent = Intent(this, LocationForegroundService::class.java).apply {
            action = ACTION_STOP
        }
        val stopPendingIntent = PendingIntent.getService(
            this, 0, stopIntent, PendingIntent.FLAG_IMMUTABLE
        )

        val notification: Notification = NotificationCompat.Builder(this, CHANNEL_ID)
            .setContentTitle("Тусик")
            .setContentText("Отображаем тебя на карте")
            .setSmallIcon(R.drawable.ic_launcher_foreground)
            .addAction(R.drawable.close, "Перестать", stopPendingIntent)
            .setContentIntent(pendingIntent)
            .build()

        startForeground(1, notification)
        serviceScope.launch {
            doBackgroundWork()
        }

        return START_NOT_STICKY
    }

    private suspend fun doBackgroundWork() {
        while (serviceScope.isActive) {
            // Выполните задачу в фоне (если нужно)
            if (ActivityCompat.checkSelfPermission(
                    applicationContext,
                    Manifest.permission.ACCESS_FINE_LOCATION
                ) != PackageManager.PERMISSION_GRANTED
            ) {
                stopSelf()
                break
            }

            val location = fusedLocationClient.getCurrentLocation(
                com.google.android.gms.location.Priority.PRIORITY_HIGH_ACCURACY,
                CancellationTokenSource().token
            ).await()

            val latitude = location.latitude
            val longitude = location.longitude
            lastLocationState.setLastLatitude(latitude.toFloat())
            lastLocationState.setLastLongitude(longitude.toFloat())
            try {
                locationControllerApi.addLocation(AddLocationDto(
                    latitude = latitude.toFloat(),
                    longitude = longitude.toFloat()
                ))
            } catch (exception: ClientException) {
                exception.printStackTrace()
            }

            delay(Duration.parse("30s"))
        }
    }

    override fun onDestroy() {
        super.onDestroy()

        serviceScope.cancel()
    }

    override fun onBind(intent: Intent?): IBinder? {
        return null
    }

    private fun createNotificationChannel() {
        val serviceChannel = NotificationChannel(
            CHANNEL_ID,
            "Foreground Service Channel",
            NotificationManager.IMPORTANCE_DEFAULT
        )
        val manager = getSystemService(NotificationManager::class.java)
        manager?.createNotificationChannel(serviceChannel)
    }

    companion object {
        const val CHANNEL_ID = "TusikLocationServiceChannel"
        const val ACTION_STOP = "STOP"
        const val ACTION_START = "START"
    }
}