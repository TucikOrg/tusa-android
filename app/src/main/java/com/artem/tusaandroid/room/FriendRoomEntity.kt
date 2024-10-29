package com.artem.tusaandroid.room

import androidx.room.Entity
import androidx.room.PrimaryKey

@Entity(tableName = "friend")
data class FriendRoomEntity(
    @PrimaryKey() val phone: String
)