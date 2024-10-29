package com.artem.tusaandroid.socket

import com.artem.tusaandroid.app.action.friends.FriendDto
import com.artem.tusaandroid.app.action.friends.FriendRequestDto
import com.artem.tusaandroid.app.avatar.AvatarDTO
import com.artem.tusaandroid.location.LocationDto
import kotlinx.serialization.ExperimentalSerializationApi
import kotlinx.serialization.cbor.Cbor
import kotlinx.serialization.decodeFromByteArray
import okio.ByteString

class ReceiveMessage() {
    val friendsBus: EventBus<List<FriendDto>> = EventBus()
    val friendAcceptedBus: EventBus<FriendDto> = EventBus()
    val friendRequestsBus: EventBus<List<FriendRequestDto>> = EventBus()
    val findUsersBus: EventBus<List<FriendDto>> = EventBus()
    val avatarBus: EventBus<AvatarDTO> = EventBus()
    val locationBus: EventBus<List<LocationDto>> = EventBus()

    @OptIn(ExperimentalSerializationApi::class)
    fun receiveBytesMessage(message: ByteString) {
        val socketBinaryMessage = Cbor.decodeFromByteArray<SocketBinaryMessage>(message.toByteArray())
        when (socketBinaryMessage.type) {
            "locations" -> {
                val locations = Cbor.decodeFromByteArray<List<LocationDto>>(socketBinaryMessage.data)
                locationBus.pushEvent(locations)
            }
            "avatar" -> {
                val avatarDTO = Cbor.decodeFromByteArray<AvatarDTO>(socketBinaryMessage.data)
                avatarBus.pushEvent(avatarDTO)
            }
            "my-friends" -> {
                val friends = Cbor.decodeFromByteArray<List<FriendDto>>(socketBinaryMessage.data)
                friendsBus.pushEvent(friends)
            }
            "accept-friend" -> {
                val friend = Cbor.decodeFromByteArray<FriendDto>(socketBinaryMessage.data)
                friendAcceptedBus.pushEvent(friend)
            }
            "friends-to-me-requests" -> {
                val requests = Cbor.decodeFromByteArray<List<FriendRequestDto>>(socketBinaryMessage.data)
                friendRequestsBus.pushEvent(requests)
            }
            "find-users" -> {
                val users = Cbor.decodeFromByteArray<List<FriendDto>>(socketBinaryMessage.data)
                findUsersBus.pushEvent(users)
            }
        }
    }
}