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
    var payload: String,
    val updateTime: Long,
    val deleted: Boolean
) {
    fun isServerUploaded() = id != null

    fun getClearedPayload(): List<String> {
        if (payload.isEmpty()) return emptyList()
        return payload.split(",")
    }

    fun getOpponentId(myId: Long): Long {
        return if (firstUserId == myId) secondUserId else firstUserId
    }
}