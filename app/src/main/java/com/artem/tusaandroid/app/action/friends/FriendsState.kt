package com.artem.tusaandroid.app.action.friends

import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import com.artem.tusaandroid.socket.EventListener
import com.artem.tusaandroid.socket.ReceiveMessage
import com.artem.tusaandroid.socket.SendMessage
import com.artem.tusaandroid.socket.SocketBinaryMessage
import kotlinx.serialization.ExperimentalSerializationApi
import kotlinx.serialization.cbor.Cbor
import kotlinx.serialization.encodeToByteArray


class FriendsState(
    private val sendMessage: SendMessage?,
    private val receiveMessage: ReceiveMessage?
) {

    init {
        receiveMessage?.friendsBus?.addListener(object : EventListener<List<FriendDto>> {
            override fun onEvent(event: List<FriendDto>) {
                friends.value = event
            }
        })
        receiveMessage?.friendRequestsBus?.addListener(object : EventListener<List<FriendRequestDto>> {
            override fun onEvent(event: List<FriendRequestDto>) {
                toMeRequests.value = event
            }
        })
        receiveMessage?.friendAcceptedBus?.addListener(object : EventListener<FriendDto> {
            override fun onEvent(event: FriendDto) {
                friends.value += event
                toMeRequests.value = toMeRequests.value.filter { it.id != event.id }
            }
        })
    }

    var friends: MutableState<List<FriendDto>> = mutableStateOf(listOf())
    var toMeRequests: MutableState<List<FriendRequestDto>> = mutableStateOf(listOf())

    fun loadFriendsAndRequests() {
        sendMessage?.sendMessage(SocketBinaryMessage("my-friends", byteArrayOf()))
        sendMessage?.sendMessage(SocketBinaryMessage("friends-to-me-requests", byteArrayOf()))
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun acceptRequest(id: Long) {
        sendMessage?.sendMessage(SocketBinaryMessage("accept-friend", Cbor.encodeToByteArray(id)))
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun addFriend(friend: FriendDto) {
        sendMessage?.sendMessage(SocketBinaryMessage("add-friend", Cbor.encodeToByteArray(friend.id)))
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun removeFriend(id: Long) {
        sendMessage?.sendMessage(SocketBinaryMessage("delete-friend", Cbor.encodeToByteArray(id)))
        friends.value = friends.value.filter { it.id != id }
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun removeRequestToFriend(id: Long) {
        sendMessage?.sendMessage(SocketBinaryMessage("delete-request", Cbor.encodeToByteArray(id)))
        toMeRequests.value = toMeRequests.value.filter { it.id != id }
    }
}