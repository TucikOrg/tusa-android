package com.artem.tusaandroid.room.messenger

import androidx.room.Entity
import androidx.room.PrimaryKey

@Entity(tableName = "unread_messages")
data class UnreadMessages(
    @PrimaryKey() var userId: Long,
    var count: Int,
)