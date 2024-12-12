package com.artem.tusaandroid.socket

import com.artem.tusaandroid.dto.AddUserDto
import com.artem.tusaandroid.dto.AllUsersRequest
import kotlinx.serialization.ExperimentalSerializationApi
import kotlinx.serialization.cbor.Cbor
import kotlinx.serialization.encodeToByteArray
import okhttp3.WebSocket
import okio.ByteString.Companion.toByteString


class SendMessage(
    private val socket: WebSocket?
) {
    // admin
    @OptIn(ExperimentalSerializationApi::class)
    fun createUser(create: AddUserDto) {
        SocketBinaryMessage("create-user", Cbor.encodeToByteArray(create)).send(socket)
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun allUsers(request: AllUsersRequest) {
        SocketBinaryMessage("all-users", Cbor.encodeToByteArray(request)).send(socket)
    }


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