package com.artem.tusaandroid.app.action.friends

import com.artem.tusaandroid.dto.FriendDto
import com.artem.tusaandroid.location.LocationsState
import com.artem.tusaandroid.socket.SocketListener
import kotlinx.serialization.ExperimentalSerializationApi


class FriendsState(
    private val socketListener: SocketListener?,
    private val locationsState: LocationsState
) {
    @OptIn(ExperimentalSerializationApi::class)
    fun addFriend(friend: FriendDto) {
        socketListener?.getSendMessage()?.addFriend(friend.id)
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun removeFriend(id: Long) {
        socketListener?.getSendMessage()?.removeFriend(id)
        locationsState.removeLocation(id)
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun removeRequestToFriend(id: Long) {
        socketListener?.getSendMessage()?.removeRequestToFriend(id)
    }
}