package com.artem.tusaandroid.dto.messenger

import kotlinx.serialization.Serializable

@Serializable
data class ChatAction(
    val chat: ChatResponse,
    val actionType: ChatsActionType,
    val actionTime: Long
)