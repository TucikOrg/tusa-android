package com.artem.tusaandroid.socket

import kotlinx.serialization.ExperimentalSerializationApi
import kotlinx.serialization.cbor.Cbor
import kotlinx.serialization.encodeToByteArray
import okhttp3.WebSocket
import okio.ByteString.Companion.toByteString


class SendMessage(
    private val socket: WebSocket?
) {
    @OptIn(ExperimentalSerializationApi::class)
    fun sendMessage(message: SocketBinaryMessage) {
        socket?.send(Cbor.encodeToByteArray(message).toByteString())
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun loadAvatar(id: Long) {
        val data = SocketBinaryMessage("avatar", Cbor.encodeToByteArray(id))
        socket?.send(Cbor.encodeToByteArray(data).toByteString())
    }

    fun loadFriendsAndRequests() {
        sendMessage(SocketBinaryMessage("my-friends", byteArrayOf()))
        sendMessage(SocketBinaryMessage("friends-to-me-requests", byteArrayOf()))
    }

    fun locations() {
        sendMessage(SocketBinaryMessage("locations", byteArrayOf()))
    }

    fun loadFriendsAvatars() {
        sendMessage(SocketBinaryMessage("friends-avatars", byteArrayOf()))
    }
}