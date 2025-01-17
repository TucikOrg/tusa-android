package com.artem.tusaandroid.dto.messenger

import com.artem.tusaandroid.dto.MessageResponse
import kotlinx.serialization.Serializable


@Serializable
data class InitMessagesResponse(
    val messages: List<MessageResponse>,
    val timePoint: Long
)