package com.artem.tusaandroid.socket

import com.artem.tusaandroid.dto.AvatarDTO
import com.artem.tusaandroid.dto.CreatedUser
import com.artem.tusaandroid.dto.FriendDto
import com.artem.tusaandroid.dto.FriendRequestDto
import com.artem.tusaandroid.dto.FriendsInitializationState
import com.artem.tusaandroid.dto.FriendsRequestsInitializationState
import com.artem.tusaandroid.dto.ImageDto
import com.artem.tusaandroid.dto.IsMyLocationVisibleStateDto
import com.artem.tusaandroid.dto.IsOnlineDto
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
    val updateLocationVisibleBus: EventBus<IsMyLocationVisibleStateDto> = EventBus()
    val locationsBus: EventBus<List<LocationDto>> = EventBus()
    val createdUser: EventBus<CreatedUser> = EventBus()
    val allUsers: EventBus<UsersPage> = EventBus()
    val refreshFriendsBus: EventBus<Unit> = EventBus()
    val refreshFriendsRequestsBus: EventBus<Unit> = EventBus()
    val friendsActionsBus: EventBus<List<FriendDto>> = EventBus()
    val friendsRequestsActionsBus: EventBus<List<FriendRequestDto>> = EventBus()
    val connectionClosedBus: EventBus<String> = EventBus()
    val isOnlineBus: EventBus<IsOnlineDto> = EventBus()
    val imageBus: EventBus<ImageDto> = EventBus()
    val pingBus: EventBus<Unit> = EventBus()

    @OptIn(ExperimentalSerializationApi::class)
    fun receiveBytesMessage(message: ByteString) {
        val socketBinaryMessage = Cbor.decodeFromByteArray<SocketBinaryMessage>(message.toByteArray())

        // обработка сообщений мессенджера
        receiveMessenger.handleMessage(socketBinaryMessage)

        when (socketBinaryMessage.type) {
            "ping" -> {
                pingBus.pushEvent(Unit)
            }
            "image" -> {
                val imageDto = Cbor.decodeFromByteArray<ImageDto>(socketBinaryMessage.data)
                imageBus.pushEvent(imageDto)
            }
            "is-online" -> {
                val isOnlineDto = Cbor.decodeFromByteArray<IsOnlineDto>(socketBinaryMessage.data)
                isOnlineBus.pushEvent(isOnlineDto)
            }
            "closed" -> {
                val reason = Cbor.decodeFromByteArray<String>(socketBinaryMessage.data)
                connectionClosedBus.pushEvent(reason)
            }
            "locations" -> {
                val locations = Cbor.decodeFromByteArray<List<LocationDto>>(socketBinaryMessage.data)
                locationsBus.pushEvent(locations)
            }
            "update-location-visible" -> {
                val updateLocationVisible = Cbor.decodeFromByteArray<IsMyLocationVisibleStateDto>(socketBinaryMessage.data)
                updateLocationVisibleBus.pushEvent(updateLocationVisible)
            }
            "update-location" -> {
                val updateLocation = Cbor.decodeFromByteArray<UpdateLocationDto>(socketBinaryMessage.data)
                updateLocationBus.pushEvent(updateLocation)
            }
            "friends-actions" -> {
                val friendsActions = Cbor.decodeFromByteArray<List<FriendDto>>(socketBinaryMessage.data)
                friendsActionsBus.pushEvent(friendsActions)
            }
            "friends-requests-actions" -> {
                val friendsRequestsActions = Cbor.decodeFromByteArray<List<FriendRequestDto>>(socketBinaryMessage.data)
                friendsRequestsActionsBus.pushEvent(friendsRequestsActions)
            }
            "refresh-friends-requests" -> {
                refreshFriendsRequestsBus.pushEvent(Unit)
            }
            "refresh-friends" -> {
                refreshFriendsBus.pushEvent(Unit)
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