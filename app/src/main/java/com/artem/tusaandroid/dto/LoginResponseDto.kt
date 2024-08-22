package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable

@Serializable
data class LoginResponseDto(
    val uniqueName: String,
    val name: String,
    val jwt: String,
    val needTransferLocationToken: Boolean
)