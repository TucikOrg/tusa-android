package com.artem.tusaandroid.app.action.friends

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.app.chat.ChatState
import com.artem.tusaandroid.dto.FriendDto
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
open class FriendRowViewModel @Inject constructor(
    private val friendsState: FriendsState,
    private val chatState: ChatState
): ViewModel() {
    fun removeFriend(id: Long) {
        friendsState.removeFriend(id)
    }

    fun openChat(dto: FriendDto) {
        chatState.openChatWithUser(dto.id, viewModelScope)
    }
}
