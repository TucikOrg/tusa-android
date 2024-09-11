package com.artem.tusaandroid.room

import androidx.room.Dao
import androidx.room.Insert
import androidx.room.OnConflictStrategy
import androidx.room.Query
import kotlinx.coroutines.flow.Flow

@Dao
interface FriendDao {
    @Insert(onConflict = OnConflictStrategy.IGNORE)
    suspend fun insert(friend: FriendEntity)

    @Query("DELETE FROM friend WHERE installAppId = :installAppId")
    suspend fun deleteByInstallAppId(installAppId: String)

    @Query("SELECT * FROM friend")
    fun getAllFriends(): Flow<List<FriendEntity>>
}