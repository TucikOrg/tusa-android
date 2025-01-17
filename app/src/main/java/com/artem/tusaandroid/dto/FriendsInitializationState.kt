package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable

@Serializable
data class FriendsInitializationState(
    val friends: List<FriendDto>,
    val timePoint: Long
)