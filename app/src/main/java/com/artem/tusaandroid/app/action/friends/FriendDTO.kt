package com.artem.tusaandroid.app.action.friends

import kotlinx.serialization.Serializable

@Serializable
data class FriendDto(
    val id: Long?,
    val name: String,
    val uniqueName: String?
)