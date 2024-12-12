package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable

@Serializable
data class AddUserDto(
    val uniqueName: String,
    val gmail: String
)