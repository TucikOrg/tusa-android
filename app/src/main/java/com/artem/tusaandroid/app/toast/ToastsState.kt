package com.artem.tusaandroid.app.toast

import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateListOf
import com.artem.tusaandroid.app.action.friends.FriendsState
import com.artem.tusaandroid.app.avatar.AvatarState
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.dto.MessageResponse
import com.artem.tusaandroid.room.FriendDao
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.launch

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
}