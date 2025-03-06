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
    val updateTime: Long,
    val deleted: Boolean
) {
    fun getChatWithId(meId: Long) : Long {
        return if (firstUserId == meId) secondUserId else firstUserId
    }
}