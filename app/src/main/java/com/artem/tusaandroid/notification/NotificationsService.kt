package com.artem.tusaandroid.notification

import android.app.NotificationManager
import android.content.Context
import android.content.Intent
import android.provider.Settings
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import androidx.core.content.ContextCompat.getSystemService

class NotificationsService {
    val showDialog: MutableState<Boolean> = mutableStateOf(false)

    fun check(context: Context): Boolean {
        if (areEnabled(context) == true) return false

        showDialog.value = true
        return true
    }

    fun getNotificationManager(context: Context): NotificationManager {
        val notificationManager = getSystemService(context, NotificationManager::class.java)!!
        return notificationManager
    }

    fun areEnabled(context: Context): Boolean {
        return getNotificationManager(context).areNotificationsEnabled()
    }

    fun goToSettings(context: Context) {
        val notificationManager = getNotificationManager(context)
        val notificationsEnabled = notificationManager.areNotificationsEnabled()
        if (notificationsEnabled == false) {
            val intent = Intent(Settings.ACTION_APP_NOTIFICATION_SETTINGS).apply {
                addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
                putExtra(Settings.EXTRA_APP_PACKAGE, context.packageName)
                putExtra(Settings.EXTRA_CHANNEL_ID, context.applicationInfo.uid)
            }
            context.startActivity(intent)
        }
    }
}