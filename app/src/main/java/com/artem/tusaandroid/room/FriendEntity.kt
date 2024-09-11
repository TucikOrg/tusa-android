package com.artem.tusaandroid.room

import androidx.room.Entity
import androidx.room.PrimaryKey

@Entity(tableName = "friend")
data class FriendEntity(
    @PrimaryKey() val installAppId: String,
    val secretKey: String,
    val phone: String
)