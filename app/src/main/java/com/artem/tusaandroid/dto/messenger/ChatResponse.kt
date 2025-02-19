package com.artem.tusaandroid.dto.messenger

import androidx.room.Entity
import androidx.room.PrimaryKey
import kotlinx.serialization.Serializable

@Serializable
@Entity(tableName = "chat")
data class ChatResponse(
    @PrimaryKey() val id: Long?,
    val firstUserId: Long,
    val secondUserId: Long,
    val firsUserName: String,
    val secondUserName: String,
    val firstUserUniqueName: String?,
    val secondUserUniqueName: String?,
)