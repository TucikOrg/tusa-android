package com.artem.tusaandroid.app.chat

import androidx.compose.foundation.lazy.LazyListState
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.app.AlineTwoLongsIds
import com.artem.tusaandroid.app.IsOnlineState
import com.artem.tusaandroid.app.action.friends.FriendsState
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.dto.FriendDto
import com.artem.tusaandroid.dto.MessageResponse
import com.artem.tusaandroid.dto.SendMessage
import com.artem.tusaandroid.dto.UsersPage
import com.artem.tusaandroid.dto.messenger.WritingMessage
import com.artem.tusaandroid.room.FriendDao
import com.artem.tusaandroid.room.messenger.MessageDao
import com.artem.tusaandroid.socket.EventListener
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
import kotlin.collections.MutableMap

@HiltViewModel
open class ChatViewModel @Inject constructor(
    val chatsState: ChatState,
    val friendsState: FriendsState,
    val socketListener: SocketListener,
    val friendDao: FriendDao,
    val profileState: ProfileState,
    val messagesDao: MessageDao,
    val isOnlineState: IsOnlineState
): ViewModel() {
    var page = 1
    private var messagesStatesMap: MutableMap<Pair<Long, Long>, StateFlow<List<MessageResponse>>> = mutableMapOf()
    private var writingMessagesMap: MutableMap<Long, MutableState<String>> = mutableMapOf()
    private var updateWritingMessagesTime: MutableMap<Long, LocalDateTime> = mutableMapOf()

    fun getIsFriendOnline(): MutableState<Boolean> {
        return isOnlineState.isUserOnline(getWithUserId())
    }

    fun getCurrentWritingMessage(): MutableState<String> {
        return getWritingMessage(getWithUserId())
    }

    fun getWritingMessage(fromUserId: Long): MutableState<String> {
        var writingMessage = writingMessagesMap[fromUserId]
        if (writingMessage == null) {
            writingMessage = mutableStateOf("")
            writingMessagesMap[fromUserId] = writingMessage
        }
        return writingMessage
    }

    fun checkWritingMessagesLife() {
        val now = LocalDateTime.now(ZoneOffset.UTC)
        updateWritingMessagesTime.forEach { (userId, time) ->
            if (now.minusSeconds(5) > time) {
                writingMessagesMap[userId]!!.value = ""
            }
        }
    }

    init {
        socketListener.getReceiveMessage().getReceiveMessenger().writingMessageBus.addListener(object : EventListener<WritingMessage> {
            override fun onEvent(data: WritingMessage) {
                if (data.toUserId != profileState.getUserId()) {
                    throw Exception("Writing message to another user")
                }
                if (writingMessagesMap[data.fromUserId] == null) {
                    writingMessagesMap[data.fromUserId] = mutableStateOf("")
                }
                updateWritingMessagesTime[data.fromUserId] = LocalDateTime.now(ZoneOffset.UTC)
                writingMessagesMap[data.fromUserId]!!.value = data.message
            }
        })


    }

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

        // временно сохраняем сообщение в базу данных
        // оно будет показываться в чате как сообщение для отправки
        viewModelScope.launch {
            messagesDao.insert(MessageResponse(
                id = null,
                temporaryId = sendMessage.temporaryId,
                firstUserId = currentChat.firstUserId,
                secondUserId = currentChat.secondUserId,
                senderId = profileState.getUserId(),
                message = message,
                creation = LocalDateTime.now(ZoneOffset.UTC).toEpochSecond(ZoneOffset.UTC)
            ))
        }
    }

    fun loadMoreItems() {
        val withUserId = getWithUserId()

        val page = getMessages().value.size / MessagesConsts.batchSize
        socketListener.getSendMessage()?.messages(
            withUserId = withUserId,
            page = page,
            size = MessagesConsts.batchSize
        )
    }

    fun writingMessage(message: String) {
        val withUserId = getWithUserId()
        socketListener.getSendMessage()?.writingMessage(withUserId, message)
    }

    private fun getWithUserId(): Long {
        val currentChat = chatsState.chat.value!!
        val withUserId = if (currentChat.firstUserId == profileState.getUserId())
            currentChat.secondUserId else currentChat.firstUserId
        return withUserId
    }
}