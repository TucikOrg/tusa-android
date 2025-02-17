package com.artem.tusaandroid.dto

import androidx.room.Entity
import androidx.room.PrimaryKey
import kotlinx.serialization.Serializable

@Entity(tableName = "friend")
@Serializable
data class FriendDto(
    @PrimaryKey()  val id: Long,
    val name: String,
    val uniqueName: String?,
    val lastOnlineTime: Long
)