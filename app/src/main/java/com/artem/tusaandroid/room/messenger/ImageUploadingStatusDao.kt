package com.artem.tusaandroid.room.messenger

import androidx.room.Dao
import androidx.room.Insert
import androidx.room.OnConflictStrategy
import androidx.room.Query
import kotlinx.coroutines.flow.Flow

@Dao
interface ImageUploadingStatusDao {
    @Query("SELECT * FROM image_uploading_status WHERE temporaryId = :temporaryId")
    suspend fun findById(temporaryId: String): ImageUploadingStatusEntity?

    @Query("SELECT * FROM image_uploading_status WHERE temporaryId = :temporaryId")
    fun findStatus(temporaryId: String): Flow<ImageUploadingStatusEntity?>

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    suspend fun insert(imageUploadingStatus: ImageUploadingStatusEntity)

    @Query("SELECT * FROM image_uploading_status WHERE temporaryId IN (:temporaryIds)")
    suspend fun findByTempIdIn(temporaryIds: List<String>): List<ImageUploadingStatusEntity>

    @Query("SELECT * FROM image_uploading_status")
    fun findAll(): List<ImageUploadingStatusEntity>
}