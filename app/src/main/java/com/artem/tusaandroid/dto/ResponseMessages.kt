package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable

@Serializable
data class ResponseMessages(
    val messages: List<MessageResponse>,
    val totalPages: Int,
    val chatId: Long,
    val page: Int
)