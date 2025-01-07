package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable

@Serializable
data class ChatResponse(
    val chatId: Long,
    val ownerId: Long,
    val toId: Long,
    val muted: Boolean,
    val lastMessage: String,
    val lastMessageOwner: Long,
)