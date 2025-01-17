package com.artem.tusaandroid.dto.messenger

import kotlinx.serialization.Serializable

@Serializable
data class InitChatsResponse(
    val chats: List<ChatResponse>,
    val timePoint: Long
)