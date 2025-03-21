package com.artem.tusaandroid.app.action.chats

import androidx.compose.foundation.lazy.LazyListState
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.app.AlineTwoLongsIds
import com.artem.tusaandroid.app.action.friends.FriendsState
import com.artem.tusaandroid.app.chat.ChatState
import com.artem.tusaandroid.app.chat.ChatsState
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.dto.FriendDto
import com.artem.tusaandroid.dto.MessageResponse
import com.artem.tusaandroid.dto.messenger.ChatResponse
import com.artem.tusaandroid.room.FriendDao
import com.artem.tusaandroid.room.messenger.ChatDao
import com.artem.tusaandroid.room.messenger.MessageDao
import com.artem.tusaandroid.room.messenger.UnreadMessagesDao
import com.artem.tusaandroid.socket.SocketListener
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.flow.SharingStarted
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.stateIn
import kotlinx.coroutines.launch
import javax.inject.Inject

@HiltViewModel
open class ChatsViewModel @Inject constructor(
    private val socketListener: SocketListener,
    private val chatsState: ChatsState,
    private val friendsState: FriendsState,
    private val chatState: ChatState,
    private val friendDao: FriendDao,
    private val chatDao: ChatDao,
    private val profileState: ProfileState,
    private val messageDao: MessageDao,
    private val unreadMessagesDao: UnreadMessagesDao
): ViewModel() {
    val showModal = mutableStateOf(false)
    val lazyListState = LazyListState()
    val chatFriend = mutableStateOf<FriendDto?>(null)
    private val lastMessageStateMap = mutableMapOf<Pair<Long, Long>, StateFlow<MessageResponse?>>()
    private val messagesCountStateMap = mutableMapOf<Long, StateFlow<Int>>()

    private val unreadMessagesCount = unreadMessagesDao.getCountFlow().stateIn(
        scope = viewModelScope,
        started = SharingStarted.WhileSubscribed(5000),
        initialValue = 0
    )

    fun getLastMessageFlow(chatResponse: ChatResponse): StateFlow<MessageResponse?> {
        val ids = AlineTwoLongsIds.aline(chatResponse.firstUserId, chatResponse.secondUserId)
        return lastMessageStateMap.getOrPut(ids) {
            createLastMessageFlow(ids)
        }
    }

    private fun createLastMessageFlow(ids: Pair<Long, Long>): StateFlow<MessageResponse?> {
        return messageDao.getLastMessageFlow(ids.first, ids.second)
            .stateIn(
                scope = viewModelScope,
                started = SharingStarted.WhileSubscribed(5000),
                initialValue = null
            )
    }

    val chats: StateFlow<List<ChatResponse>> = chatDao.getAllFlow()
        .stateIn(
            scope = viewModelScope,
            started = SharingStarted.WhileSubscribed(5000),
            initialValue = emptyList()
        )

    fun openChat(chat: ChatResponse) {
        viewModelScope.launch {
            chatState.openChat(chat)
        }
    }

    fun getMeId() = profileState.getUserId()

    fun dismiss() {
        showModal.value = false
    }

    fun getUnreadMessagesOfChatCount(chat: ChatResponse): StateFlow<Int> {
        val userId = if (chat.firstUserId == profileState.getUserId()) chat.secondUserId else chat.firstUserId

        var state = messagesCountStateMap[userId]
        if (state == null) {
            state = unreadMessagesDao.getUnreadMessagesCountFlow(userId)
                .stateIn(
                    scope = viewModelScope,
                    started = SharingStarted.WhileSubscribed(5000),
                    initialValue = 0
                )
            messagesCountStateMap[userId] = state
        }

        return state
    }

    fun getUnreadMessagesCount(): StateFlow<Int> {
        return unreadMessagesCount
    }
}