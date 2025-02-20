package com.artem.tusaandroid.app.image

import androidx.room.Dao
import androidx.room.Insert
import androidx.room.OnConflictStrategy
import androidx.room.Query
import com.artem.tusaandroid.dto.FriendDto

@Dao
interface TempIdToUriDao {
    @Insert(onConflict = OnConflictStrategy.REPLACE)
    suspend fun insert(tempIdToUri: TempIdToUriEntity)

    @Query("SELECT * FROM temp_id_to_uri WHERE tempLocalId = :tempLocalId")
    suspend fun findById(tempLocalId: String): TempIdToUriEntity?
}