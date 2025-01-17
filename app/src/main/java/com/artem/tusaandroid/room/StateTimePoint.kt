package com.artem.tusaandroid.room

import androidx.room.Entity
import androidx.room.PrimaryKey

@Entity(tableName = "state_time_point")
data class StateTimePoint(
    @PrimaryKey() val stateType: Int,
    val timePoint: Long
)