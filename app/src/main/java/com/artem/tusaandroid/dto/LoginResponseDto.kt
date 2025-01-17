package com.artem.tusaandroid.dto

data class LoginResponseDto(
    val id: Long,
    val uniqueName: String,
    val name: String,
    val jwt: String,
    val phone: String,
    val gmail: String,
    val pictureUrl: String
)