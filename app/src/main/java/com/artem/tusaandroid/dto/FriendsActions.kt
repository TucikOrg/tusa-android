package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable

@Serializable
data class FriendsActions(
    val fromUserId: Long,
    val toUserId: Long,
    val actionType: FriendsActionType,
    val actionTime: Long
) {
    val id: Long? = null
}