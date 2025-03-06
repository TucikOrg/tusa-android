package com.artem.tusaandroid.app.beauty

import androidx.room.Entity
import androidx.room.PrimaryKey

@Entity(tableName = "recently_used_gif")
data class RecentlyUsedGif(
    @PrimaryKey() var url: String,
    var usedTime: Long
)