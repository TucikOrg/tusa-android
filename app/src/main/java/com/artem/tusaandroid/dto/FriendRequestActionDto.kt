package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable


@Serializable
data class FriendRequestActionDto(
    val friendRequestDto: FriendRequestDto,
    val friendsActionType: FriendsActionType,
    val actionTime: Long
)