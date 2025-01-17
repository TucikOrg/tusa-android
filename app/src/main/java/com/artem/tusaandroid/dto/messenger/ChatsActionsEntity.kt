package com.artem.tusaandroid.dto.messenger

import kotlinx.serialization.Serializable

@Serializable
data class ChatsActionsEntity(
    val firstUserId: Long,
    val secondUserId: Long,
    val actionType: ChatsActionType,
    val actionTime: Long
) {
    val id: Long? = null
}