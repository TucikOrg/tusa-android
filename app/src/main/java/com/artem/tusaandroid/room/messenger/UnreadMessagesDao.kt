package com.artem.tusaandroid.room.messenger

import androidx.room.Dao
import androidx.room.Insert
import androidx.room.OnConflictStrategy
import androidx.room.Query
import kotlinx.coroutines.flow.Flow


@Dao
interface UnreadMessagesDao {
    @Insert(onConflict = OnConflictStrategy.REPLACE)
    suspend fun insert(unreadMessages: UnreadMessages)

    @Query("SELECT * FROM unread_messages")
    fun getAllFlow(): Flow<List<UnreadMessages>>

    @Query("SELECT * FROM unread_messages WHERE userId = :userId")
    suspend fun findByUserId(userId: Long): UnreadMessages?

    @Query("SELECT sum(unread_messages.count) FROM unread_messages")
    fun getCountFlow(): Flow<Int>

    @Query("SELECT unread_messages.count FROM unread_messages WHERE userId = :userId")
    fun getUnreadMessagesCountFlow(userId: Long): Flow<Int>
}