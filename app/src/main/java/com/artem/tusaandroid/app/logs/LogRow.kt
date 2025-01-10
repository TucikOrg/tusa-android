package com.artem.tusaandroid.app.logs

import java.time.LocalDateTime

data class LogRow(
    val time: LocalDateTime,
    val message: String,
    val reason: String
)