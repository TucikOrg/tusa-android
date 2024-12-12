package com.artem.tusaandroid.socket

import com.artem.tusaandroid.app.action.friends.FriendDto
import com.artem.tusaandroid.app.action.friends.FriendRequestDto
import com.artem.tusaandroid.app.avatar.AvatarDTO
import com.artem.tusaandroid.dto.CreatedUser
import com.artem.tusaandroid.dto.User
import com.artem.tusaandroid.dto.UsersPage
import com.artem.tusaandroid.location.LocationDto
import kotlinx.serialization.ExperimentalSerializationApi
import kotlinx.serialization.cbor.Cbor
import kotlinx.serialization.decodeFromByteArray
import okio.ByteString

class ReceiveMessage() {
    val friendsBus: EventBus<List<FriendDto>> = EventBus()
    val friendAcceptedBus: EventBus<FriendDto> = EventBus()
    val friendRequestsBus: EventBus<List<FriendRequestDto>> = EventBus()
    val newFriendRequestBus: EventBus<FriendRequestDto> = EventBus()
    val findUsersBus: EventBus<List<FriendDto>> = EventBus()
    val avatarBus: EventBus<AvatarDTO> = EventBus()
    val locationsBus: EventBus<List<LocationDto>> = EventBus()
    val iAmAcceptedBus: EventBus<FriendDto> = EventBus()
    val deleteFriendBus: EventBus<Long> = EventBus()
    val iWasDeletedFromFriendsBus: EventBus<Long> = EventBus()
    val deleteRequestBus: EventBus<Long> = EventBus()
    val createdUser: EventBus<CreatedUser> = EventBus()
    val allUsers: EventBus<UsersPage> = EventBus()

    @OptIn(ExperimentalSerializationApi::class)
    fun receiveBytesMessage(message: ByteString) {
        val socketBinaryMessage = Cbor.decodeFromByteArray<SocketBinaryMessage>(message.toByteArray())
        when (socketBinaryMessage.type) {
            // admins
            "created-user" -> {
                val data = Cbor.decodeFromByteArray<CreatedUser>(socketBinaryMessage.data)
                createdUser.pushEvent(data)
            }
            "all-users" -> {
                val usersPage = Cbor.decodeFromByteArray<UsersPage>(socketBinaryMessage.data)
                allUsers.pushEvent(usersPage)
            }

            "locations" -> {
                val locations = Cbor.decodeFromByteArray<List<LocationDto>>(socketBinaryMessage.data)
                locationsBus.pushEvent(locations)
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
                // я принял заявку в друзья
                val friend = Cbor.decodeFromByteArray<FriendDto>(socketBinaryMessage.data)
                friendAcceptedBus.pushEvent(friend)
            }
            "i-am-accepted" -> {
                // моя заявка в друзья принята
                val friend = Cbor.decodeFromByteArray<FriendDto>(socketBinaryMessage.data)
                iAmAcceptedBus.pushEvent(friend)
            }
            "friends-to-me-requests" -> {
                val requests = Cbor.decodeFromByteArray<List<FriendRequestDto>>(socketBinaryMessage.data)
                friendRequestsBus.pushEvent(requests)
            }
            "find-users" -> {
                val users = Cbor.decodeFromByteArray<List<FriendDto>>(socketBinaryMessage.data)
                findUsersBus.pushEvent(users)
            }
            "add-friend" -> {
                // мне пришла заявка в друзья
                val requestToMe = Cbor.decodeFromByteArray<FriendRequestDto>(socketBinaryMessage.data)
                newFriendRequestBus.pushEvent(requestToMe)
            }
            "delete-friend" -> {
                // я удалил друга
                val friendId = Cbor.decodeFromByteArray<Long>(socketBinaryMessage.data)
                deleteFriendBus.pushEvent(friendId)
            }
            "i-was-deleted-from-friends" -> {
                // меня удалили из друзей
                val friendId = Cbor.decodeFromByteArray<Long>(socketBinaryMessage.data)
                iWasDeletedFromFriendsBus.pushEvent(friendId)
            }
            "delete-request" -> {
                // я удалил заявку в друзья
                val friendId = Cbor.decodeFromByteArray<Long>(socketBinaryMessage.data)
                deleteRequestBus.pushEvent(friendId)
            }
        }
    }
}