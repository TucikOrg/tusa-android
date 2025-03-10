package com.artem.tusaandroid.app.toast

import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateListOf
import com.artem.tusaandroid.app.action.friends.FriendsState
import com.artem.tusaandroid.app.avatar.AvatarState
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.dto.FriendDto
import com.artem.tusaandroid.dto.FriendRequestDto
import com.artem.tusaandroid.dto.MessageResponse
import com.artem.tusaandroid.room.FriendDao
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.launch
import java.time.LocalDateTime
import java.time.ZoneOffset

class ToastsState(
    val profileState: ProfileState,
    val avatarState: AvatarState,
    val friendsDao: FriendDao
) {
    private var toastCounter = mutableIntStateOf(0)
    val toasts = mutableStateListOf<ToastItem>()

    fun add(
        title: String,
        message: String,
        avatarId: Long
    ) {
        toastCounter.intValue++
        toasts.add(
            ToastItem(
                id = toastCounter.intValue,
                avatarId = avatarId,
                title = title,
                message = message
            )
        )
    }

    fun newMessage(message: MessageResponse, viewModelScope: CoroutineScope) {
        viewModelScope.launch {
            val currentTime = LocalDateTime.now(ZoneOffset.UTC).toEpochSecond(ZoneOffset.UTC)
            val difference = currentTime - message.updateTime
            if (difference > 10) {
                // если сообщение старое то не надо показывать уведомление о нем
                return@launch
            }

            // берем имя друга чтобы показать уведомление
            val senderId = message.senderId
            val friend = friendsDao.findById(senderId)
            if (friend == null) return@launch
            val title = friend.getTitleOfFriend()

            add(
                title = title,
                message = message.message,
                avatarId = senderId
            )
        }
    }

    fun newFriend(friend: FriendDto, viewModelScope: CoroutineScope) {
        viewModelScope.launch {
            val currentTime = LocalDateTime.now(ZoneOffset.UTC).toEpochSecond(ZoneOffset.UTC)
            val difference = currentTime - friend.updateTime
            if (difference > 10) {
                // если друг уже давно добавлен то не надо показывать уведомление о нем
                return@launch
            }

            val friendTitle = friend.getTitleOfFriend()
            add(
                title = "Новый друг",
                message = friendTitle,
                avatarId = friend.id
            )
        }
    }

    fun newFriendRequest(friendRequest: FriendRequestDto, viewModelScope: CoroutineScope) {
        viewModelScope.launch {
            val currentTime = LocalDateTime.now(ZoneOffset.UTC).toEpochSecond(ZoneOffset.UTC)
            val difference = currentTime - friendRequest.updateTime
            if (difference > 10) {
                // если запрос уже давно отправлен то не надо показывать уведомление о нем
                return@launch
            }

            val titleOfUser = friendRequest.getTitleOf()
            add(
                title = "Запрос в друзья",
                message = titleOfUser,
                avatarId = friendRequest.userId
            )
        }
    }
}