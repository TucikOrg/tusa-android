package com.artem.tusaandroid.room

import androidx.room.Database
import androidx.room.RoomDatabase

@Database(entities = [FriendRoomEntity::class], version = 1, exportSchema = false)
abstract class FriendDatabase: RoomDatabase() {
    abstract fun friendDao(): FriendDao
}