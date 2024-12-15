package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable

@Serializable
data class ChangeNameOther(
    val userId: Long,
    val name: String
)