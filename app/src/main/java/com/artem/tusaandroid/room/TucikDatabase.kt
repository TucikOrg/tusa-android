package com.artem.tusaandroid.room

import androidx.room.Database
import androidx.room.RoomDatabase
import com.artem.tusaandroid.app.beauty.RecentlyUsedGif
import com.artem.tusaandroid.app.beauty.RecentlyUsedGifsDao
import com.artem.tusaandroid.app.image.ImageDao
import com.artem.tusaandroid.app.image.ImageEntity
import com.artem.tusaandroid.app.image.TempIdToUriDao
import com.artem.tusaandroid.app.image.TempIdToUriEntity
import com.artem.tusaandroid.dto.FriendDto
import com.artem.tusaandroid.dto.FriendRequestDto
import com.artem.tusaandroid.dto.MessageResponse
import com.artem.tusaandroid.dto.messenger.ChatResponse
import com.artem.tusaandroid.room.messenger.ChatDao
import com.artem.tusaandroid.room.messenger.ImageUploadingStatusDao
import com.artem.tusaandroid.room.messenger.ImageUploadingStatusEntity
import com.artem.tusaandroid.room.messenger.MessageDao
import com.artem.tusaandroid.room.messenger.UnreadMessages
import com.artem.tusaandroid.room.messenger.UnreadMessagesDao

@Database(
    entities = [
        FriendDto::class,
        AvatarRoomEntity::class,
        StateTimePoint::class,
        FriendRequestDto::class,
        ChatResponse::class,
        MessageResponse::class,
        TempIdToUriEntity::class,
        ImageEntity::class,
        ImageUploadingStatusEntity::class,
        RecentlyUsedGif::class,
        UnreadMessages::class
    ], version = 1, exportSchema = false)
abstract class TucikDatabase: RoomDatabase() {
    abstract fun friendDao(): FriendDao
    abstract fun avatarDao(): AvatarDao
    abstract fun statesDao(): StatesTimePointDao
    abstract fun friendRequestDao(): FriendRequestDao
    abstract fun chatDao(): ChatDao
    abstract fun messageDao(): MessageDao
    abstract fun tempLocalIdToUriDao(): TempIdToUriDao
    abstract fun imageDao(): ImageDao
    abstract fun imageUploadingStatusDao(): ImageUploadingStatusDao
    abstract fun recentlyUsedGifDao(): RecentlyUsedGifsDao
    abstract fun unreadMessagesDao(): UnreadMessagesDao
}