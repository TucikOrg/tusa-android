package com.artem.tusaandroid.room

import android.content.Context
import androidx.room.Room
import com.artem.tusaandroid.app.image.ImageDao
import com.artem.tusaandroid.app.image.TempIdToUriDao
import com.artem.tusaandroid.room.messenger.ChatDao
import com.artem.tusaandroid.room.messenger.MessageDao
import dagger.Module
import dagger.Provides
import dagger.hilt.InstallIn
import dagger.hilt.android.qualifiers.ApplicationContext
import dagger.hilt.components.SingletonComponent
import javax.inject.Singleton

@Module
@InstallIn(SingletonComponent::class)
object DatabaseModule {
    @Provides
    @Singleton
    fun provideDatabase(@ApplicationContext appContext: Context): TucikDatabase {
        return Room.databaseBuilder(
            appContext,
            TucikDatabase::class.java,
            "tucik_database"
        ).allowMainThreadQueries().build()
    }

    @Provides
    fun provideTempLocalIdToUriDao(database: TucikDatabase): TempIdToUriDao {
        return database.tempLocalIdToUriDao()
    }

    @Provides
    fun provideImageDao(database: TucikDatabase): ImageDao {
        return database.imageDao()
    }

    @Provides
    fun provideFriendsDao(database: TucikDatabase): FriendDao {
        return database.friendDao()
    }

    @Provides
    fun provideAvatarDao(database: TucikDatabase): AvatarDao {
        return database.avatarDao()
    }

    @Provides
    fun provideStatesDao(database: TucikDatabase): StatesTimePointDao {
        return database.statesDao()
    }

    @Provides
    fun provideFriendRequestDao(database: TucikDatabase): FriendRequestDao {
        return database.friendRequestDao()
    }

    @Provides
    fun provideChatDao(database: TucikDatabase): ChatDao {
        return database.chatDao()
    }

    @Provides
    fun provideMessageDao(database: TucikDatabase): MessageDao {
        return database.messageDao()
    }
}