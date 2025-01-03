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
import com.artem.tusaandroid.MainActivity
import com.artem.tusaandroid.R
import com.artem.tusaandroid.await
import com.artem.tusaandroid.requests.CustomTucikEndpoints
import com.google.android.gms.location.FusedLocationProviderClient
import com.google.android.gms.location.LocationServices
import com.google.android.gms.tasks.CancellationTokenSource
import com.squareup.moshi.Moshi
import com.squareup.moshi.adapter
import dagger.hilt.android.AndroidEntryPoint
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.cancel
import kotlinx.coroutines.delay
import kotlinx.coroutines.isActive
import kotlinx.coroutines.launch
import okhttp3.MediaType.Companion.toMediaTypeOrNull
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody.Companion.toRequestBody
import javax.inject.Inject
import kotlin.time.Duration

@AndroidEntryPoint
class LocationForegroundService: Service() {
    private lateinit var fusedLocationClient: FusedLocationProviderClient
    private val serviceScope = CoroutineScope(Dispatchers.IO)

    @Inject
    lateinit var lastLocationState: LastLocationState
    @Inject
    lateinit var okHttpClient: OkHttpClient
    @Inject
    lateinit var moshi: Moshi
    @Inject
    lateinit var customTucikEndpoints: CustomTucikEndpoints

    override fun onStartCommand(intent: Intent, flags: Int, startId: Int): Int {
        // нужно грохнуть сервис сбора локации
        if (intent.action == ACTION_STOP) {
            stopSelf()
            lastLocationState.saveLocationForegroundServiceStarted(false)
            return START_NOT_STICKY
        }

        lastLocationState.saveLocationForegroundServiceStarted(true)
        fusedLocationClient = LocationServices.getFusedLocationProviderClient(applicationContext)

        // Показывать уведомление о том что запущено обновление местоположения
        val notification = buildNotification()
        startForeground(1, notification)

        serviceScope.launch {
            doBackgroundWork()
        }

        return START_NOT_STICKY
    }

    override fun onCreate() {
        super.onCreate()

        createNotificationChannel()
    }

    override fun onDestroy() {
        super.onDestroy()

        serviceScope.cancel()
    }

    override fun onBind(intent: Intent?): IBinder? {
        return null
    }


    @OptIn(ExperimentalStdlibApi::class)
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
                } catch (exception: Exception) {
                    exception.printStackTrace()
                }
            }

            delay(Duration.parse("30s"))
        }
    }

    private fun createNotificationChannel() {
        val serviceChannel = NotificationChannel(
            CHANNEL_ID,
            "Foreground Service Channel",
            NotificationManager.IMPORTANCE_HIGH
        )
        val manager = getSystemService(NotificationManager::class.java)
        manager?.createNotificationChannel(serviceChannel)
    }

    private fun buildNotification(): Notification {
        val notificationIntent = Intent(this, MainActivity::class.java)
        val pendingIntent = PendingIntent.getActivity(
            this,
            0,
            notificationIntent,
            PendingIntent.FLAG_IMMUTABLE // Use FLAG_IMMUTABLE for API 31+
        )

        val stopIntent = Intent(this, LocationForegroundService::class.java).apply {
            action = ACTION_STOP
        }
        val stopPendingIntent = PendingIntent.getService(
            this, 0, stopIntent, PendingIntent.FLAG_IMMUTABLE
        )

        return NotificationCompat.Builder(this, CHANNEL_ID)
            .setContentTitle("Локация в Тусике")
            .setContentText("Обновляем твое местоположение")
            .setSmallIcon(R.drawable.favorite)
            .setContentIntent(pendingIntent)
            .setPriority(NotificationCompat.PRIORITY_HIGH)
            .addAction(R.drawable.close, "Остановить", stopPendingIntent)
            .build()
    }

    companion object {
        const val CHANNEL_ID = "TusikLocationServiceChannel"
        const val ACTION_STOP = "STOP"
        const val ACTION_START = "START"
    }
}