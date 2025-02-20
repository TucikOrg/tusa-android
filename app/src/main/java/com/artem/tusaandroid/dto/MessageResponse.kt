package com.artem.tusaandroid.dto

import androidx.room.Entity
import androidx.room.PrimaryKey
import kotlinx.serialization.Serializable

@Serializable
@Entity(tableName = "message")
data class MessageResponse(
    val id: Long?,
    @PrimaryKey() var temporaryId: String,
    var firstUserId: Long,
    var secondUserId: Long,
    var senderId: Long,
    val message: String,
    var creation: Long,
    var payload: String
) {
    fun isServerUploaded() = id != null
}