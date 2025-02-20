package com.artem.tusaandroid.room

import androidx.room.Dao
import androidx.room.Insert
import androidx.room.OnConflictStrategy
import androidx.room.Query

@Dao
interface AvatarDao {

    @Query("SELECT * FROM avatar WHERE id = :id")
    fun getAvatarById(id: Long): AvatarRoomEntity?

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    suspend fun insert(avatar: AvatarRoomEntity)

    @Query("DELETE FROM avatar WHERE id = :id")
    fun deleteById(id: Long)
}