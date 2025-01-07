package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable

@Serializable
data class RequestMessages(
    val chatId: Long,
    val page: Int,
    val size: Int
)