package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable

@Serializable
data class FriendActionDto(
    val friendDto: FriendDto,
    val friendsActionType: FriendsActionType,
    val actionTime: Long
)