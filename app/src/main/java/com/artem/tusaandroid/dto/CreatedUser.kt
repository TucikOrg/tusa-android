package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable

@Serializable
data class CreatedUser(
    val name: String?,
    val uniqueName: String?,
    val id: Long
)