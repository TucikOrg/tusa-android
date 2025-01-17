package com.artem.tusaandroid.dto.messenger

import kotlinx.serialization.Serializable

@Serializable
data class MessagesActionsEntity(
    val firstUserId: Long,
    val secondUserId: Long,
    val senderId: Long,
    val actionType: MessagesActionType,
    val actionTime: Long
) {
    val id: Long? = null
}