package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable

@Serializable
data class MessageResponse(
    var ownerId: Long,
    var toId: Long,
    var chatId: Long,
    var payload: List<Long>,
    val message: String,
    val creation: Long,
    val deletedOwner: Boolean,
    val deletedTo: Boolean,
    val changed: Boolean,
    val read: Boolean,
)