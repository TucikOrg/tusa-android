package com.artem.tusaandroid.dto

import androidx.annotation.Keep
import com.squareup.moshi.Json

@Keep
data class AddLocationDto(
    @Json(name = "latitude") val latitude: Float,
    @Json(name = "longitude") val longitude: Float
)