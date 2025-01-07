package com.artem.tusaandroid.app.action.chats

import androidx.compose.foundation.lazy.LazyListState
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.app.action.friends.FriendDto
import com.artem.tusaandroid.app.action.friends.FriendsState
import com.artem.tusaandroid.app.chat.ChatState
import com.artem.tusaandroid.app.chat.ChatsState
import com.artem.tusaandroid.dto.ChatResponse
import com.artem.tusaandroid.socket.SocketListener
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
open class ChatsViewModel @Inject constructor(
    private val socketListener: SocketListener?,
    private val chatsState: ChatsState?,
    private val friendsState: FriendsState?,
    private val chatState: ChatState?
): ViewModel() {
    val showModal = mutableStateOf(false)
    val lazyListState = LazyListState()

    fun openChat(chat: ChatResponse) {
        chatState?.openChat(chat)
    }

    fun getChats(): MutableState<List<ChatResponse>> {
        return chatsState?.chats?: mutableStateOf(emptyList())
    }

    fun getChatFriend(chat: ChatResponse): FriendDto? {
        return friendsState?.friends?.value?.find { it.id == chat.toId }
    }

    fun dismiss() {
        showModal.value = false
    }

    fun loadChats() {
        chatsState?.loadChats()
    }
}