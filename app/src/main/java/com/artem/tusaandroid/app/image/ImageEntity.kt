package com.artem.tusaandroid.app.image

import androidx.room.Entity
import androidx.room.PrimaryKey

@Entity(tableName = "image")
data class ImageEntity(
     @PrimaryKey() val tempId: String,
    val image: ByteArray
)