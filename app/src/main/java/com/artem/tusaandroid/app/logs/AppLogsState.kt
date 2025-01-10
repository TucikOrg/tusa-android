package com.artem.tusaandroid.app.logs

import androidx.compose.runtime.mutableStateOf
import java.time.LocalDateTime

class AppLogsState {
    private var logs = mutableStateOf(listOf<LogRow>())

    fun getLogs() = logs

    fun addRow(message: String, reason: String, allowDuplicate: Boolean = true) {
        if (allowDuplicate == false) {
            val lastRow = logs.value.lastOrNull()
            if (lastRow != null && lastRow.message == message && lastRow.reason == reason) {
                return
            }
        }
        logs.value += LogRow(
            time = LocalDateTime.now(),
            message = message,
            reason = reason
        )
    }
}