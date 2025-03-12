package com.artem.tusaandroid.firebase

import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.PendingIntent
import android.content.Intent
import android.os.Build
import androidx.core.app.NotificationCompat
import com.artem.tusaandroid.MainActivity
import com.google.firebase.messaging.FirebaseMessagingService
import com.google.firebase.messaging.RemoteMessage
import com.artem.tusaandroid.R

class MyFirebaseMessagingServiceNew : FirebaseMessagingService() {
    private fun createNotificationChannel() {
        val channelId = "messages_channel"
        val channelName = "Messages"
        val importance = NotificationManager.IMPORTANCE_HIGH
        val channel = NotificationChannel(channelId, channelName, importance).apply {
            description = "Channel for message notifications"
        }
        val notificationManager = getSystemService(NotificationManager::class.java)
        notificationManager.createNotificationChannel(channel)
    }

    override fun onMessageReceived(remoteMessage: RemoteMessage) {
        remoteMessage.data.let { data ->
            val receivedTitle = data["title"] // Получаем title с бэкенда (опционально)
            val body = data["body"]
            if (body != null) {
                showNotification(receivedTitle, body) // Используем "1" как title
            }
        }
    }

    override fun onNewToken(token: String) {
        super.onNewToken(token)
    }

    override fun onCreate() {
        super.onCreate()
        createNotificationChannel() // Создаём канал при старте сервиса
    }

    companion object {
        private val messageMap = mutableMapOf<String, MutableList<String>>()
    }

    private fun showNotification(title: String?, message: String?) {
        if (title == null || message == null) return

        val notificationManager = getSystemService(NOTIFICATION_SERVICE) as NotificationManager
        val groupKey = "group_$title" // Уникальная группа для каждого title
        val summaryId = title.hashCode() // Фиксированный ID для сводки по title

        // Добавляем сообщение в список для данного title
        val messages = messageMap.getOrPut(title) { mutableListOf() }
        messages.add(message)

        // Создаем стиль Inbox для сводного уведомления
        val inboxStyle = NotificationCompat.InboxStyle()
            .setBigContentTitle("$title: ${messages.size} новых сообщений")
        messages.forEach { inboxStyle.addLine(it) }

        val intent = Intent(this, MainActivity::class.java).apply {
            putExtra("notification_title", title)
            putExtra("notification_message", message)
            flags = Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TASK
        }
        val pendingIntent = PendingIntent.getActivity(
            this,
            0, // requestCode, можно оставить 0
            intent,
            PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_IMMUTABLE
        )

        // Создаем сводное уведомление
        val summaryBuilder = NotificationCompat.Builder(this, "messages_channel")
            .setSmallIcon(R.drawable.favorite)
            .setContentTitle(title)
            .setContentText("У вас есть ${messages.size} новых сообщений")
            .setPriority(NotificationCompat.PRIORITY_HIGH)
            .setAutoCancel(true)
            .setGroup(groupKey)
            .setGroupSummary(true)
            .setStyle(inboxStyle)
            .setContentIntent(pendingIntent)

        // Публикуем сводное уведомление
        notificationManager.notify(summaryId, summaryBuilder.build())
    }
}