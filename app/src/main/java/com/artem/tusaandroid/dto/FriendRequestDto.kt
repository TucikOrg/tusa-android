package com.artem.tusaandroid.dto

import androidx.room.Entity
import androidx.room.PrimaryKey
import kotlinx.serialization.Serializable

@Serializable
@Entity(tableName = "friend_request")
data class FriendRequestDto(
    @PrimaryKey val userId: Long,
    val userName: String,
    val userUniqueName: String?,
    val isRequestSender: Boolean
)