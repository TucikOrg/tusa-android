package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable


@Serializable
data class RequestLastMessages(
    val chatId: Long,
    val pageSize: Int,
)