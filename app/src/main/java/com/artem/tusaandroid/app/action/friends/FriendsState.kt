package com.artem.tusaandroid.app.action.friends

import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import com.artem.tusaandroid.NativeLibrary
import com.artem.tusaandroid.socket.EventListener
import com.artem.tusaandroid.socket.ReceiveMessage
import com.artem.tusaandroid.socket.SendMessage
import com.artem.tusaandroid.socket.SocketBinaryMessage
import com.artem.tusaandroid.socket.SocketListener
import kotlinx.serialization.ExperimentalSerializationApi
import kotlinx.serialization.cbor.Cbor
import kotlinx.serialization.encodeToByteArray


class FriendsState(
    private val socketListener: SocketListener?,
) {

    init {
        socketListener?.getReceiveMessage()?.friendsBus?.addListener(object : EventListener<List<FriendDto>> {
            override fun onEvent(event: List<FriendDto>) {
                friends.value = event
            }
        })
        socketListener?.getReceiveMessage()?.friendRequestsBus?.addListener(object : EventListener<List<FriendRequestDto>> {
            override fun onEvent(event: List<FriendRequestDto>) {
                toMeRequests.value = event
            }
        })
        socketListener?.getReceiveMessage()?.friendAcceptedBus?.addListener(object : EventListener<FriendDto> {
            override fun onEvent(event: FriendDto) {
                friends.value += event
                toMeRequests.value = toMeRequests.value.filter { it.id != event.id }
            }
        })
        socketListener?.getReceiveMessage()?.iAmAcceptedBus?.addListener(object : EventListener<FriendDto> {
            override fun onEvent(event: FriendDto) {
                friends.value += event
            }
        })
        socketListener?.getReceiveMessage()?.deleteFriendBus?.addListener(object : EventListener<Long> {
            override fun onEvent(event: Long) {
                friends.value = friends.value.filter { it.id != event }
            }
        })
        socketListener?.getReceiveMessage()?.deleteRequestBus?.addListener(object : EventListener<Long> {
            override fun onEvent(event: Long) {
                toMeRequests.value = toMeRequests.value.filter { it.id != event }
            }
        })
        socketListener?.getReceiveMessage()?.iWasDeletedFromFriendsBus?.addListener(object : EventListener<Long> {
            override fun onEvent(event: Long) {
                friends.value = friends.value.filter { it.id != event }
            }
        })
        socketListener?.getReceiveMessage()?.newFriendRequestBus?.addListener(object : EventListener<FriendRequestDto> {
            override fun onEvent(event: FriendRequestDto) {
                toMeRequests.value += event
            }
        })
    }

    var friends: MutableState<List<FriendDto>> = mutableStateOf(listOf())
    var toMeRequests: MutableState<List<FriendRequestDto>> = mutableStateOf(listOf())



    @OptIn(ExperimentalSerializationApi::class)
    fun acceptRequest(id: Long) {
        socketListener?.getSendMessage()?.sendMessage(SocketBinaryMessage("accept-friend", Cbor.encodeToByteArray(id)))
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun addFriend(friend: FriendDto) {
        socketListener?.getSendMessage()?.sendMessage(SocketBinaryMessage("add-friend", Cbor.encodeToByteArray(friend.id)))
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun removeFriend(id: Long) {
        socketListener?.getSendMessage()?.sendMessage(SocketBinaryMessage("delete-friend", Cbor.encodeToByteArray(id)))
        friends.value = friends.value.filter { it.id != id }
        NativeLibrary.removeMarker(id)
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun removeRequestToFriend(id: Long) {
        socketListener?.getSendMessage()?.sendMessage(SocketBinaryMessage("delete-request", Cbor.encodeToByteArray(id)))
        toMeRequests.value = toMeRequests.value.filter { it.id != id }
    }
}