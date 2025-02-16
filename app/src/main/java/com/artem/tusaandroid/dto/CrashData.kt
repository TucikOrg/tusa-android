package com.artem.tusaandroid.dto

import androidx.annotation.Keep
import com.squareup.moshi.Json

@Keep
data class CrashData(
    @Json(name = "message") val message: String,
    @Json(name = "stackTrace") val stackTrace: String,
    @Json(name = "thread") val thread: String
)