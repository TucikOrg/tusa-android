package com.artem.tusaandroid.room

import androidx.room.Database
import androidx.room.RoomDatabase
import com.artem.tusaandroid.dto.FriendDto
import com.artem.tusaandroid.dto.FriendRequestDto
import com.artem.tusaandroid.dto.MessageResponse
import com.artem.tusaandroid.dto.messenger.ChatResponse
import com.artem.tusaandroid.room.messenger.ChatDao
import com.artem.tusaandroid.room.messenger.MessageDao

@Database(
    entities = [
        FriendDto::class,
        AvatarRoomEntity::class,
        StateTimePoint::class,
        FriendRequestDto::class,
        ChatResponse::class,
        MessageResponse::class
    ], version = 1, exportSchema = false)
abstract class TucikDatabase: RoomDatabase() {
    abstract fun friendDao(): FriendDao
    abstract fun avatarDao(): AvatarDao
    abstract fun statesDao(): StatesTimePointDao
    abstract fun friendRequestDao(): FriendRequestDao
    abstract fun chatDao(): ChatDao
    abstract fun messageDao(): MessageDao
}