package com.artem.tusaandroid.app.image

import androidx.room.Entity
import androidx.room.PrimaryKey

@Entity(tableName = "temp_id_to_uri")
data class TempIdToUriEntity(
    @PrimaryKey() val tempLocalId: String,
    val uri: String
)