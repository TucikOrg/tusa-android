package com.artem.tusaandroid.socket

import com.artem.tusaandroid.dto.AddUserDto
import com.artem.tusaandroid.dto.AllUsersRequest
import com.artem.tusaandroid.dto.ChangeNameOther
import com.artem.tusaandroid.dto.FakeLocation
import com.artem.tusaandroid.dto.RequestChats
import com.artem.tusaandroid.dto.RequestMessages
import com.artem.tusaandroid.dto.SendMessage
import com.artem.tusaandroid.dto.messenger.InitMessenger
import com.artem.tusaandroid.room.StateTimePoint
import kotlinx.serialization.ExperimentalSerializationApi
import kotlinx.serialization.cbor.Cbor
import kotlinx.serialization.encodeToByteArray
import okhttp3.WebSocket
import okio.ByteString.Companion.toByteString


class SendMessage(
    private val socket: WebSocket?
) {
    @OptIn(ExperimentalSerializationApi::class)
    fun removeFriend(id: Long) {
        sendMessage(SocketBinaryMessage("delete-friend", Cbor.encodeToByteArray(id)))
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun getMessages(chatId: Long, page: Int, size: Int) {
        val data = SocketBinaryMessage("messages", Cbor.encodeToByteArray(RequestMessages(chatId, page, size)))
        sendMessage(data)
    }

    // admin
    @OptIn(ExperimentalSerializationApi::class)
    fun fakeLocation(latitude: Float, longitude: Float, userId: Long) {
        SocketBinaryMessage("fake-location", Cbor.encodeToByteArray(FakeLocation(latitude, longitude, userId))).send(socket)
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun createRequestToMe(fromUserId: Long) {
        SocketBinaryMessage("create-request-to-me", Cbor.encodeToByteArray(fromUserId)).send(socket)
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun createFriends(withUserId: Long) {
        SocketBinaryMessage("force-friends", Cbor.encodeToByteArray(withUserId)).send(socket)
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun createUser(create: AddUserDto) {
        SocketBinaryMessage("create-user", Cbor.encodeToByteArray(create)).send(socket)
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun allUsers(request: AllUsersRequest) {
        SocketBinaryMessage("all-users", Cbor.encodeToByteArray(request)).send(socket)
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun changeName(name: String, userId: Long) {
        val data = ChangeNameOther(userId, name)
        sendMessage(SocketBinaryMessage("change-name-other", Cbor.encodeToByteArray(data)))
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun changeUniqueName(name: String, userId: Long) {
        val data = ChangeNameOther(userId, name)
        sendMessage(SocketBinaryMessage("change-unique-name-other", Cbor.encodeToByteArray(data)))
    }


    @OptIn(ExperimentalSerializationApi::class)
    fun sendMessage(message: SocketBinaryMessage) {
        socket?.send(Cbor.encodeToByteArray(message).toByteString())
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun loadAvatar(id: Long) {
        sendMessage(SocketBinaryMessage("avatar", Cbor.encodeToByteArray(id)))
    }

    fun loadFriends() {
        sendMessage(SocketBinaryMessage("my-friends", byteArrayOf()))
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun findChatWithUser(userid: Long) {
        sendMessage(SocketBinaryMessage("find-chat", Cbor.encodeToByteArray(userid)))
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun sendChatMessage(sendMessage: SendMessage) {
        sendMessage(SocketBinaryMessage("send-message", Cbor.encodeToByteArray(sendMessage)))
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun loadChats(page: Int, size: Int) {
        sendMessage(SocketBinaryMessage(
            "chats",
            Cbor.encodeToByteArray(RequestChats(page, size))
        ))
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun friendsActions(stateTimePoint: StateTimePoint) {
        sendMessage(SocketBinaryMessage(
            "friends-actions",
            Cbor.encodeToByteArray(stateTimePoint.timePoint)
        ))
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun friendsRequestsActions(stateTimePoint: StateTimePoint) {
        sendMessage(SocketBinaryMessage(
            "friends-requests-actions",
            Cbor.encodeToByteArray(stateTimePoint.timePoint)
        ))
    }

    fun loadFriendsRequests() {
        sendMessage(SocketBinaryMessage("friends-requests", byteArrayOf()))
    }

    fun locations() {
        sendMessage(SocketBinaryMessage("locations", byteArrayOf()))
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun messagesActions(point: StateTimePoint) {
        sendMessage(SocketBinaryMessage("messages-actions", Cbor.encodeToByteArray(point.timePoint)))
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun chatsActions(point: StateTimePoint) {
        sendMessage(SocketBinaryMessage("chats-actions", Cbor.encodeToByteArray(point.timePoint)))
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun initMessages(init: InitMessenger) {
        sendMessage(SocketBinaryMessage("init-messages", Cbor.encodeToByteArray(init)))
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun initChats(init: InitMessenger) {
        sendMessage(SocketBinaryMessage("init-chats", Cbor.encodeToByteArray(init)))
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun acceptFriend(id: Long) {
        sendMessage(SocketBinaryMessage("accept-friend", Cbor.encodeToByteArray(id)))
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun addFriend(id: Long) {
        sendMessage(SocketBinaryMessage("add-friend", Cbor.encodeToByteArray(id)))
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun removeRequestToFriend(id: Long) {
        sendMessage(SocketBinaryMessage("delete-request", Cbor.encodeToByteArray(id)))
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun avatarsActions(point: StateTimePoint) {
        sendMessage(SocketBinaryMessage("avatars-actions", Cbor.encodeToByteArray(point.timePoint)))
    }
}