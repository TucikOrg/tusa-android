package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable

@Serializable
data class FriendsRequestsInitializationState(
    val friends: List<FriendRequestDto>,
    val timePoint: Long
)