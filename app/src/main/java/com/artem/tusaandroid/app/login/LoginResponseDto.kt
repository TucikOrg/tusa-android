package com.artem.tusaandroid.app.login

data class LoginResponseDto(
    val id: Long,
    val uniqueName: String,
    val name: String,
    val jwt: String,
    val phone: String
)