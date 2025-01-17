package com.artem.tusaandroid.room

import androidx.room.Dao
import androidx.room.Insert
import androidx.room.OnConflictStrategy
import androidx.room.Query
import com.artem.tusaandroid.dto.FriendDto
import kotlinx.coroutines.flow.Flow

@Dao
interface FriendDao {
    @Query("SELECT * FROM friend")
    fun getAllFlow(): Flow<List<FriendDto>>

    @Insert(onConflict = OnConflictStrategy.IGNORE)
    suspend fun insert(friend: FriendDto)

    @Insert(onConflict = OnConflictStrategy.IGNORE)
    suspend fun insertAll(friends: List<FriendDto>)

    @Query("DELETE FROM friend WHERE id = :id")
    suspend fun deleteById(id: Long)

    @Query("SELECT * FROM friend WHERE id = :id")
    suspend fun findById(id: Long): FriendDto?

    @Query("SELECT * FROM friend ORDER BY id ASC LIMIT :limit OFFSET :offset")
    suspend fun getPage(offset: Int, limit: Int): List<FriendDto>

    @Query("DELETE FROM friend")
    suspend fun clearAll()

    @Query("SELECT COUNT(*) FROM friend")
    suspend fun count(): Int
}