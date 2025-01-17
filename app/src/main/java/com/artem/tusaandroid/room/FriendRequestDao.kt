package com.artem.tusaandroid.room

import androidx.room.Dao
import androidx.room.Insert
import androidx.room.OnConflictStrategy
import androidx.room.Query
import com.artem.tusaandroid.dto.FriendRequestDto
import kotlinx.coroutines.flow.Flow

@Dao
interface FriendRequestDao {
    @Query("SELECT * FROM friend_request WHERE userId = :userId")
    suspend fun findById(userId: Long): FriendRequestDto?

    @Query("SELECT * FROM friend_request")
    fun getAllFlow(): Flow<List<FriendRequestDto>>

    @Query("SELECT * FROM friend_request WHERE isRequestSender = true")
    fun getRequestsToMeFlow(): Flow<List<FriendRequestDto>>

    @Insert(onConflict = OnConflictStrategy.IGNORE)
    suspend fun insert(friend: FriendRequestDto)

    @Query("DELETE FROM friend_request WHERE userId = :userId")
    suspend fun deleteById(userId: Long)

    @Query("DELETE FROM friend_request")
    suspend fun clearAll()

    @Query("SELECT COUNT(*) FROM friend_request")
    suspend fun count(): Int

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    suspend fun insertAll(requests: List<FriendRequestDto>)
}