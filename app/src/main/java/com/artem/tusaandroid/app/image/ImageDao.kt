package com.artem.tusaandroid.app.image

import androidx.room.Dao
import androidx.room.Insert
import androidx.room.OnConflictStrategy
import androidx.room.Query

@Dao
interface ImageDao {
    @Insert(onConflict = OnConflictStrategy.REPLACE)
    suspend fun insert(image: ImageEntity)

    @Query("SELECT * FROM image WHERE tempId = :tempId")
    suspend fun findById(tempId: String): ImageEntity?
}