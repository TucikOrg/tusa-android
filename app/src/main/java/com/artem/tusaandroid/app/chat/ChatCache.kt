package com.artem.tusaandroid.app.chat

import com.artem.tusaandroid.dto.MessageResponse

data class ChatCache(
    val messages: List<MessageResponse> = listOf(),
    val loadSize: Int = 30,
    var page: Int = 0,
    var totalPages: Int = 1,
)