package com.artem.tusaandroid.dto.messenger

import com.artem.tusaandroid.dto.MessageResponse
import kotlinx.serialization.Serializable

@Serializable
data class InitMessengerResponse(
    val messages: List<MessageResponse>,
    val chats: List<ChatResponse>,
    val timePoint: Long
)