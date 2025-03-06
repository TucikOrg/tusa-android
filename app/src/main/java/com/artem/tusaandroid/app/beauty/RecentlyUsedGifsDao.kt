package com.artem.tusaandroid.app.beauty

import androidx.room.Dao
import androidx.room.Insert
import androidx.room.OnConflictStrategy
import androidx.room.Query
import kotlinx.coroutines.flow.Flow

@Dao
interface RecentlyUsedGifsDao {
    @Insert(onConflict = OnConflictStrategy.REPLACE)
    suspend fun insert(recentlyUsedGif: RecentlyUsedGif)

    @Query("SELECT * FROM recently_used_gif ORDER BY usedTime DESC")
    fun getAllFlow(): Flow<List<RecentlyUsedGif>>
}