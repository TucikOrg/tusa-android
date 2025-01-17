package com.artem.tusaandroid.socket

import com.artem.tusaandroid.dto.AvatarAction
import com.artem.tusaandroid.dto.AvatarDTO
import com.artem.tusaandroid.dto.CreatedUser
import com.artem.tusaandroid.dto.FriendActionDto
import com.artem.tusaandroid.dto.FriendDto
import com.artem.tusaandroid.dto.FriendRequestActionDto
import com.artem.tusaandroid.dto.FriendsInitializationState
import com.artem.tusaandroid.dto.FriendsRequestsInitializationState
import com.artem.tusaandroid.dto.LocationDto
import com.artem.tusaandroid.dto.UpdateLocationDto
import com.artem.tusaandroid.dto.UsersPage
import kotlinx.serialization.ExperimentalSerializationApi
import kotlinx.serialization.cbor.Cbor
import kotlinx.serialization.decodeFromByteArray
import okio.ByteString

class ReceiveMessage() {
    private val receiveMessenger = ReceiveMessenger()

    fun getReceiveMessenger(): ReceiveMessenger {
        return receiveMessenger
    }

    val friendRequestsBus: EventBus<FriendsRequestsInitializationState> = EventBus()
    val friendsBus: EventBus<FriendsInitializationState> = EventBus()
    val findUsersBus: EventBus<List<FriendDto>> = EventBus()
    val avatarBus: EventBus<AvatarDTO> = EventBus()
    val updateLocationBus: EventBus<UpdateLocationDto> = EventBus()
    val locationsBus: EventBus<List<LocationDto>> = EventBus()
    val createdUser: EventBus<CreatedUser> = EventBus()
    val allUsers: EventBus<UsersPage> = EventBus()
    val refreshFriendsBus: EventBus<Unit> = EventBus()
    val refreshFriendsRequestsBus: EventBus<Unit> = EventBus()
    val refreshAvatarsBus: EventBus<Unit> = EventBus()
    val friendsActionsBus: EventBus<List<FriendActionDto>> = EventBus()
    val friendsRequestsActionsBus: EventBus<List<FriendRequestActionDto>> = EventBus()
    val avatarsActionsBus: EventBus<List<AvatarAction>> = EventBus()

    @OptIn(ExperimentalSerializationApi::class)
    fun receiveBytesMessage(message: ByteString) {
        val socketBinaryMessage = Cbor.decodeFromByteArray<SocketBinaryMessage>(message.toByteArray())

        // обработка сообщений мессенджера
        receiveMessenger.handleMessage(socketBinaryMessage)

        when (socketBinaryMessage.type) {
            "locations" -> {
                val locations = Cbor.decodeFromByteArray<List<LocationDto>>(socketBinaryMessage.data)
                locationsBus.pushEvent(locations)
            }
            "update-location" -> {
                val updateLocation = Cbor.decodeFromByteArray<UpdateLocationDto>(socketBinaryMessage.data)
                updateLocationBus.pushEvent(updateLocation)
            }
            "friends-actions" -> {
                val friendsActions = Cbor.decodeFromByteArray<List<FriendActionDto>>(socketBinaryMessage.data)
                friendsActionsBus.pushEvent(friendsActions)
            }
            "friends-requests-actions" -> {
                val friendsRequestsActions = Cbor.decodeFromByteArray<List<FriendRequestActionDto>>(socketBinaryMessage.data)
                friendsRequestsActionsBus.pushEvent(friendsRequestsActions)
            }
            "avatars-actions" -> {
                val avatarDTO = Cbor.decodeFromByteArray<List<AvatarAction>>(socketBinaryMessage.data)
                avatarsActionsBus.pushEvent(avatarDTO)
            }
            "refresh-friends-requests" -> {
                refreshFriendsRequestsBus.pushEvent(Unit)
            }
            "refresh-friends" -> {
                refreshFriendsBus.pushEvent(Unit)
            }
            "refresh-avatars" -> {
                refreshAvatarsBus.pushEvent(Unit)
            }

            // admins
            "created-user" -> {
                val data = Cbor.decodeFromByteArray<CreatedUser>(socketBinaryMessage.data)
                createdUser.pushEvent(data)
            }
            "all-users" -> {
                val usersPage = Cbor.decodeFromByteArray<UsersPage>(socketBinaryMessage.data)
                allUsers.pushEvent(usersPage)
            }
            "avatar" -> {
                val avatarDTO = Cbor.decodeFromByteArray<AvatarDTO>(socketBinaryMessage.data)
                avatarBus.pushEvent(avatarDTO)
            }
            "my-friends" -> {
                val friends = Cbor.decodeFromByteArray<FriendsInitializationState>(socketBinaryMessage.data)
                friendsBus.pushEvent(friends)
            }
            "find-users" -> {
                val users = Cbor.decodeFromByteArray<List<FriendDto>>(socketBinaryMessage.data)
                findUsersBus.pushEvent(users)
            }
            "friends-requests" -> {
                val requests = Cbor.decodeFromByteArray<FriendsRequestsInitializationState>(socketBinaryMessage.data)
                friendRequestsBus.pushEvent(requests)
            }
        }
    }
}