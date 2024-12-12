package com.artem.tusaandroid.socket

import kotlinx.serialization.ExperimentalSerializationApi
import kotlinx.serialization.Serializable
import kotlinx.serialization.cbor.Cbor
import kotlinx.serialization.encodeToByteArray
import okhttp3.WebSocket
import okio.ByteString
import okio.ByteString.Companion.toByteString

@Serializable
class SocketBinaryMessage(
    val type: String,
    val data: ByteArray
) {
    @OptIn(ExperimentalSerializationApi::class)
    fun prepareForSend(): ByteString {
        return Cbor.encodeToByteArray(this).toByteString()
    }

    fun send(socket: WebSocket?) {
        socket?.send(prepareForSend())
    }
}