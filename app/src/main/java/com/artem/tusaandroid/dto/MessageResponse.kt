package com.artem.tusaandroid.dto

import androidx.room.Entity
import androidx.room.PrimaryKey
import kotlinx.serialization.Serializable

@Serializable
@Entity(tableName = "message")
data class MessageResponse(
    @PrimaryKey() val id: Long?,
    var firstUserId: Long,
    var secondUserId: Long,
    var senderId: Long,
    val message: String,
    val creation: Long,
)