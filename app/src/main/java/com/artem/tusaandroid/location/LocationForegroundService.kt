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
import android.util.Log
import androidx.core.app.ActivityCompat
import androidx.core.app.NotificationCompat
import com.artem.tusaandroid.MainActivity
import com.artem.tusaandroid.R
import com.artem.tusaandroid.app.logs.AppLogsState
import com.artem.tusaandroid.requests.CustomTucikEndpoints
import com.squareup.moshi.Moshi
import dagger.hilt.android.AndroidEntryPoint
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.cancel
import kotlinx.coroutines.delay
import kotlinx.coroutines.isActive
import okhttp3.OkHttpClient
import javax.inject.Inject
import kotlin.time.Duration
import com.google.android.gms.location.ActivityRecognition
import com.google.android.gms.location.ActivityRecognitionClient
import com.google.android.gms.location.ActivityTransition
import com.google.android.gms.location.ActivityTransitionRequest
import com.google.android.gms.location.ActivityTransitionResult
import com.google.android.gms.location.DetectedActivity
import kotlinx.coroutines.launch

@AndroidEntryPoint
class LocationForegroundService: Service() {
    private var started = false
    private val serviceScope = CoroutineScope(Dispatchers.IO)

    private var activityRecognition: ActivityRecognitionClient? = null
    private var forActivityPendingIntent: PendingIntent? = null

    private var makeLocationUpdates: Boolean = false
    private var currentActivityMode: Int = DetectedActivity.UNKNOWN

    @Inject
    lateinit var logsState: AppLogsState
    @Inject
    lateinit var locationUpdatingState: LocationUpdatingState
    @Inject
    lateinit var lastLocationState: LastLocationState
    @Inject
    lateinit var moshi: Moshi
    @Inject
    lateinit var customTucikEndpoints: CustomTucikEndpoints

    fun stop() {
        stopSelf()
        removeActivityRecognition()
    }

    override fun onStartCommand(intent: Intent, flags: Int, startId: Int): Int {
        if (intent.action == ACTION_START && started) {
            return START_STICKY
        }

        started = true
        activityRecognition = ActivityRecognition.getClient(this)

        // нужно грохнуть текущий сервис
        if (intent.action == ACTION_STOP) {
            logsState.addRow("Stop LocationForegroundService", "")
            lastLocationState.saveLocationForegroundServiceStarted(false)

            stop()
            return START_STICKY
        }

        if (intent.action == ACTION_ACTIVITY_RESULT) {
            Log.i("Tucik", "Activity result")
            if (ActivityTransitionResult.hasResult(intent)) {
                val result = ActivityTransitionResult.extractResult(intent)!!
                for (event in result.transitionEvents) {
                    Log.i("Tucik", "Transition: ${event.activityType} ${event.transitionType}")
                }
                val lastActivity = result.transitionEvents.last()
                val activityType = lastActivity.activityType
                val isEnter = lastActivity.transitionType == 0
                val logIsEnter = if (isEnter) "enter" else "exit"
                currentActivityMode = activityType
                when(activityType) {
                    DetectedActivity.IN_VEHICLE -> {
                        makeLocationUpdates = isEnter
                        logsState.addRow("Activity: IN_VEHICLE ($logIsEnter)", "")
                    }
                    DetectedActivity.WALKING -> {
                        makeLocationUpdates = isEnter
                        logsState.addRow("Activity: WALKING ($logIsEnter)", "")
                    }
                    DetectedActivity.STILL -> {
                        makeLocationUpdates = !isEnter
                        logsState.addRow("Activity: STILL ($logIsEnter)", "")
                    }
                }
            }
            return START_STICKY
        }

        lastLocationState.saveLocationForegroundServiceStarted(true)

        // Показывать уведомление о том что запущено обновление местоположения
        val notification = buildNotification()
        startForeground(1, notification)

        val activityIntent = Intent(this, LocationForegroundService::class.java).apply {
            action = ACTION_ACTIVITY_RESULT
        }
        forActivityPendingIntent = PendingIntent.getService(
            this,
            0,
            activityIntent,
            PendingIntent.FLAG_MUTABLE
        )

        addActivityRecognition()

        serviceScope.launch {
            doBackgroundWork()
        }
        return START_STICKY
    }

    override fun onCreate() {
        super.onCreate()
        createNotificationChannel()
    }

    override fun onDestroy() {
        super.onDestroy()
        forActivityPendingIntent?.cancel()
        serviceScope.cancel()
    }

    override fun onBind(intent: Intent?): IBinder? {
        return null
    }

    @OptIn(ExperimentalStdlibApi::class)
    private suspend fun doBackgroundWork() {
        locationUpdatingState.updateLocation("Init", true)

        while (serviceScope.isActive) {
            if (ActivityCompat.checkSelfPermission(
                    applicationContext,
                    Manifest.permission.ACCESS_FINE_LOCATION
                ) != PackageManager.PERMISSION_GRANTED
            ) {
                stop()
                break
            }

            if (makeLocationUpdates == false) {
                logsState.addRow("LocationUpdating is ignoring", "Mode=" + getActivityModeString(currentActivityMode), allowDuplicate = false)
                delay(Duration.parse("10s"))
                continue
            }

            locationUpdatingState.updateLocation(getActivityModeString(currentActivityMode))
            val interval = Duration.parse(locationUpdatingState.getUpdateLocationInterval())
            logsState.addRow("Wait for $interval", "")
            delay(interval)
        }
    }

    private fun addActivityRecognition() {
        if (ActivityCompat.checkSelfPermission(
                this,
                Manifest.permission.ACTIVITY_RECOGNITION
            ) == PackageManager.PERMISSION_GRANTED
        ) {
            val transitions = mutableListOf<ActivityTransition>()
            transitions +=
                ActivityTransition.Builder()
                    .setActivityType(DetectedActivity.IN_VEHICLE)
                    .setActivityTransition(ActivityTransition.ACTIVITY_TRANSITION_ENTER)
                    .build()
            transitions +=
                ActivityTransition.Builder()
                    .setActivityType(DetectedActivity.IN_VEHICLE)
                    .setActivityTransition(ActivityTransition.ACTIVITY_TRANSITION_EXIT)
                    .build()
            transitions +=
                ActivityTransition.Builder()
                    .setActivityType(DetectedActivity.WALKING)
                    .setActivityTransition(ActivityTransition.ACTIVITY_TRANSITION_ENTER)
                    .build()
            transitions +=
                ActivityTransition.Builder()
                    .setActivityType(DetectedActivity.WALKING)
                    .setActivityTransition(ActivityTransition.ACTIVITY_TRANSITION_EXIT)
                    .build()
            transitions +=
                ActivityTransition.Builder()
                    .setActivityType(DetectedActivity.STILL)
                    .setActivityTransition(ActivityTransition.ACTIVITY_TRANSITION_ENTER)
                    .build()
            transitions +=
                ActivityTransition.Builder()
                    .setActivityType(DetectedActivity.STILL)
                    .setActivityTransition(ActivityTransition.ACTIVITY_TRANSITION_EXIT)
                    .build()
            val request = ActivityTransitionRequest(transitions)

            activityRecognition!!
                .requestActivityTransitionUpdates(request, forActivityPendingIntent!!).addOnSuccessListener {
                    Log.i("Tucik", "Activity recognition request success")
                }.addOnFailureListener {
                    Log.e("Tucik", "Activity recognition request failed", it)
                }
        }
    }

    private fun removeActivityRecognition() {
        if (ActivityCompat.checkSelfPermission(
                this,
                Manifest.permission.ACTIVITY_RECOGNITION
            ) == PackageManager.PERMISSION_GRANTED
        ) {
            activityRecognition!!.removeActivityTransitionUpdates(forActivityPendingIntent!!)
                .addOnSuccessListener {
                    forActivityPendingIntent!!.cancel()
                }
                .addOnFailureListener { e: Exception ->
                    Log.e("Tucik", e.message!!)
                }
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
            flags = Intent.FLAG_ACTIVITY_SINGLE_TOP or Intent.FLAG_ACTIVITY_CLEAR_TOP
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

    fun getActivityModeString(mode: Int): String {
        return when(mode) {
            DetectedActivity.IN_VEHICLE -> "IN_VEHICLE"
            DetectedActivity.WALKING -> "WALKING"
            DetectedActivity.STILL -> "STILL"
            DetectedActivity.UNKNOWN -> "UNKNOWN"
            else -> "UNKNOWN"
        }
    }

    companion object {
        const val CHANNEL_ID = "TusikLocationServiceChannel"
        const val ACTION_STOP = "STOP"
        const val ACTION_START = "START"
        const val ACTION_ACTIVITY_RESULT = "ACTIVITY_RESULT"
    }
}