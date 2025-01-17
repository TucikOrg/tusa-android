package com.artem.tusaandroid.room.messenger

import androidx.room.Dao
import androidx.room.Insert
import androidx.room.OnConflictStrategy
import androidx.room.Query
import com.artem.tusaandroid.dto.messenger.ChatResponse
import kotlinx.coroutines.flow.Flow

@Dao
interface ChatDao {
    @Query("SELECT * FROM chat")
    fun getAllFlow(): Flow<List<ChatResponse>>

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    suspend fun insert(chat: ChatResponse)

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    suspend fun insertAll(chats: List<ChatResponse>)

    @Query("DELETE FROM chat WHERE id = :id")
    suspend fun deleteById(id: Long)

    @Query("SELECT * FROM chat WHERE id = :id")
    suspend fun findById(id: Long): ChatResponse?

    @Query("SELECT * FROM chat WHERE firstUserId = :firstUserId and secondUserId = :secondUserId")
    suspend fun findChatByUsers(firstUserId: Long, secondUserId: Long): ChatResponse?

    @Query("DELETE FROM chat")
    suspend fun clearAll()

    @Query("SELECT COUNT(*) FROM chat")
    suspend fun count(): Int
}