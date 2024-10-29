package com.artem.tusaandroid.app.action.friends

import kotlinx.serialization.Serializable

@Serializable
data class FriendRequestDto(
    val id: Long?,
    val name: String,
    val uniqueName: String?
)