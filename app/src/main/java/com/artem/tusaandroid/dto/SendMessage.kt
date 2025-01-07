package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable

@Serializable
data class SendMessage(
    val toId: Long,
    val message: String,
    val payload: List<Long>
)