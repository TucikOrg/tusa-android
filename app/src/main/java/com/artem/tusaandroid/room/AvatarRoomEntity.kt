package com.artem.tusaandroid.room

import androidx.room.Entity
import androidx.room.PrimaryKey

@Entity(tableName = "avatar")
data class AvatarRoomEntity(
    @PrimaryKey() val id: Long,
    val avatar: ByteArray?,
    val updatingTime: Long
)