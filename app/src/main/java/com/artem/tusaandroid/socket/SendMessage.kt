package com.artem.tusaandroid.socket

import kotlinx.serialization.ExperimentalSerializationApi
import kotlinx.serialization.cbor.Cbor
import kotlinx.serialization.encodeToByteArray


class SendMessage(
    private val webSocketClient: WebSocketClient?
) {
    @OptIn(ExperimentalSerializationApi::class)
    fun sendMessage(message: SocketBinaryMessage) {
        webSocketClient?.sendMessage(Cbor.encodeToByteArray(message))
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun loadAvatar(id: Long) {
        webSocketClient?.sendMessage(Cbor.encodeToByteArray(SocketBinaryMessage("avatar", Cbor.encodeToByteArray(id))))
    }
}