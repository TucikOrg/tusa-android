package com.artem.tusaandroid.dto

import androidx.annotation.Keep
import com.squareup.moshi.Json

@Keep
data class LoginResponseDto(
    @Json(name = "id") val id: Long,
    @Json(name = "uniqueName") val uniqueName: String,
    @Json(name = "name") val name: String,
    @Json(name = "jwt") val jwt: String,
    @Json(name = "phone") val phone: String,
    @Json(name = "gmail") val gmail: String,
    @Json(name = "pictureUrl") val pictureUrl: String
)