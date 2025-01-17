package com.artem.tusaandroid.dto.messenger

import com.artem.tusaandroid.dto.MessageResponse
import kotlinx.serialization.Serializable

@Serializable
data class MessagesAction(
    val message: MessageResponse,
    val actionType: MessagesActionType,
    val actionTime: Long
)