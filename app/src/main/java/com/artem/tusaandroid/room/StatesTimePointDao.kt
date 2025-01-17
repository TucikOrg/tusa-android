package com.artem.tusaandroid.room

import androidx.room.Dao
import androidx.room.Insert
import androidx.room.OnConflictStrategy
import androidx.room.Query

@Dao
interface StatesTimePointDao {
    @Query("SELECT * FROM state_time_point WHERE stateType = :stateType")
    suspend fun getStateTimePointByType(stateType: Int): StateTimePoint?

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    suspend fun insert(stateTimePoint: StateTimePoint)
}