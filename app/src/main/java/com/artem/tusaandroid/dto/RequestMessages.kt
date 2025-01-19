package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable

@Serializable
data class RequestMessages(
    val withUserId: Long,
    val page: Int,
    val size: Int,
)