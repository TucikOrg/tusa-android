package com.artem.tusaandroid.app.chat

import androidx.compose.foundation.lazy.LazyListState
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.app.AlineTwoLongsIds
import com.artem.tusaandroid.app.action.friends.FriendsState
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.dto.FriendDto
import com.artem.tusaandroid.dto.MessageResponse
import com.artem.tusaandroid.dto.SendMessage
import com.artem.tusaandroid.room.FriendDao
import com.artem.tusaandroid.room.messenger.MessageDao
import com.artem.tusaandroid.socket.SocketListener
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.flow.SharingStarted
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.stateIn
import kotlinx.coroutines.launch
import java.time.LocalDateTime
import java.time.ZoneOffset
import java.util.UUID
import javax.inject.Inject

@HiltViewModel
open class ChatViewModel @Inject constructor(
    val chatsState: ChatState,
    val friendsState: FriendsState,
    val socketListener: SocketListener,
    val friendDao: FriendDao,
    val profileState: ProfileState,
    val messagesDao: MessageDao
): ViewModel() {
    var page = 1
    private var messagesStatesMap: MutableMap<Pair<Long, Long>, StateFlow<List<MessageResponse>>> = mutableMapOf()


    fun getMessages(): StateFlow<List<MessageResponse>> {
        val chat = chatsState.chat.value!!
        val ids = AlineTwoLongsIds.aline(chat.firstUserId, chat.secondUserId)
        val key = Pair(ids.first, ids.second)
        var messages = messagesStatesMap[key]
        if (messages == null) {
            messages = createMessagesFlow()
            messagesStatesMap[key] = messages
        }
        return messages
    }

    private fun createMessagesFlow(): StateFlow<List<MessageResponse>> {
        val chat = chatsState.chat.value!!
        val ids = AlineTwoLongsIds.aline(chat.firstUserId, chat.secondUserId)
        return messagesDao.getAllFlow(ids.first, ids.second).stateIn(
            scope = viewModelScope,
            started = SharingStarted.WhileSubscribed(5000),
            initialValue = emptyList()
        )
    }

    val chatFriend: MutableState<FriendDto?> = mutableStateOf(null)

    fun getModalOpened() = chatsState.modalOpened
    fun getChat() = chatsState.chat

    fun closeChat() {
        chatsState.closeChat()
    }

    fun getChatFriend() {
        val meId = profileState.getUserId()
        val chat = getChat().value!!
        val chatOpponentId = if (chat.firstUserId == meId) chat.secondUserId else chat.firstUserId
        viewModelScope.launch {
            chatFriend.value = friendDao.findById(chatOpponentId)
        }
    }

    fun sendMessage(message: String) {
        val currentChat = chatsState.chat.value!!
        val toId = if (currentChat.firstUserId == profileState.getUserId())
            currentChat.secondUserId else currentChat.firstUserId
        val sendMessage = SendMessage(
            toId = toId,
            message = message,
            payload = listOf(),
            temporaryId = UUID.randomUUID().toString()
        )
        socketListener.getSendMessage()?.sendChatMessage(sendMessage)

        viewModelScope.launch {
            messagesDao.insert(MessageResponse(
                id = null,
                temporaryId = sendMessage.temporaryId,
                firstUserId = currentChat.firstUserId,
                secondUserId = currentChat.secondUserId,
                senderId = profileState.getUserId(),
                message = message,
                creation = LocalDateTime.now().toEpochSecond(ZoneOffset.UTC)
            ))
        }
    }

    fun loadMoreItems() {
        val currentChat = chatsState.chat.value!!
        val withUserId = if (currentChat.firstUserId == profileState.getUserId())
            currentChat.secondUserId else currentChat.firstUserId

        val page = getMessages().value.size / MessagesConsts.batchSize
        socketListener.getSendMessage()?.messages(
            withUserId = withUserId,
            page = page,
            size = MessagesConsts.batchSize
        )
    }
}