package com.artem.tusaandroid.room

import android.content.Context
import androidx.room.Room
import dagger.Module
import dagger.Provides
import dagger.hilt.InstallIn
import dagger.hilt.android.qualifiers.ActivityContext
import dagger.hilt.android.qualifiers.ApplicationContext
import dagger.hilt.components.SingletonComponent
import javax.inject.Singleton

@Module
@InstallIn(SingletonComponent::class)
object DatabaseModule {

    @Provides
    @Singleton
    fun provideDatabase(@ApplicationContext appContext: Context): FriendDatabase {
        return Room.databaseBuilder(
            appContext,
            FriendDatabase::class.java,
            "friend_database"
        ).build()
    }

    @Provides
    fun provideUserDao(database: FriendDatabase): FriendDao {
        return database.friendDao()
    }

    @Provides
    fun provideUserRepository(friendDao: FriendDao): FriendRepository {
        return FriendRepository(friendDao)
    }
}