package com.artem.tusaandroid.room.messenger

import androidx.room.Dao
import androidx.room.Insert
import androidx.room.OnConflictStrategy
import androidx.room.Query
import com.artem.tusaandroid.dto.MessageResponse
import kotlinx.coroutines.flow.Flow

@Dao
interface MessageDao {
    @Query("DELETE FROM message WHERE firstUserId = :firstUserId AND secondUserId = :secondUserId")
    fun deleteByFirstUserIdAndSecondUserId(firstUserId: Long, secondUserId: Long)

    @Query("SELECT * FROM message ORDER BY updateTime DESC")
    fun getAllFlow(): Flow<List<MessageResponse>>

    @Query("SELECT * FROM message WHERE firstUserId = :firstUserId AND secondUserId = :secondUserId ORDER BY updateTime DESC")
    fun getAllFlow(firstUserId: Long, secondUserId: Long): Flow<List<MessageResponse>>

    @Query("SELECT * FROM message where firstUserId = :firstUserId AND secondUserId = :secondUserId ORDER BY updateTime DESC limit 1")
    fun getLastMessageFlow(firstUserId: Long, secondUserId: Long): Flow<MessageResponse?>

    @Query("SELECT * FROM message where id is null")
    suspend fun getUnsentMessages(): List<MessageResponse>

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    suspend fun insert(message: MessageResponse)

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    suspend fun insertAll(messages: List<MessageResponse>)

    @Query("DELETE FROM message WHERE id = :id")
    suspend fun deleteById(id: Long)

    @Query("SELECT * FROM message WHERE id = :id")
    suspend fun findById(id: Long): MessageResponse?

    @Query("DELETE FROM message")
    suspend fun clearAll()

    @Query("SELECT COUNT(*) FROM message")
    suspend fun count(): Int

    @Query("SELECT * FROM message WHERE temporaryId = :temporaryId")
    suspend fun findByTempId(temporaryId: String): MessageResponse?
}