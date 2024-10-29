package com.artem.tusaandroid.socket

import kotlinx.serialization.Serializable

@Serializable
class SocketBinaryMessage(
    val type: String,
    val data: ByteArray
)