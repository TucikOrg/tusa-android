package com.artem.tusaandroid.room.messenger

import androidx.room.Entity
import androidx.room.PrimaryKey

@Entity(tableName = "image_uploading_status")
class ImageUploadingStatusEntity(
    @PrimaryKey() var temporaryId: String,
    val status: Int,
)