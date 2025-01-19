package com.artem.tusaandroid.dto


data class CrashData(
    val message: String,
    val stackTrace: String,
    val thread: String
)