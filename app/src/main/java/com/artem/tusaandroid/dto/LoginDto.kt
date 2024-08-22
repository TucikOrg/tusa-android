package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable

@Serializable
data class LoginDto(
    val phone: String,
    val code: String,
    val device: String
)