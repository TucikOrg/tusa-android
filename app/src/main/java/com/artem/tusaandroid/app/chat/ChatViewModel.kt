package com.artem.tusaandroid.app.chat

import androidx.compose.foundation.lazy.LazyListState
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.app.action.friends.FriendDto
import com.artem.tusaandroid.app.action.friends.FriendsState
import com.artem.tusaandroid.dto.ChatResponse
import com.artem.tusaandroid.dto.MessageResponse
import com.artem.tusaandroid.dto.ResponseMessages
import com.artem.tusaandroid.dto.SendMessage
import com.artem.tusaandroid.socket.EventListener
import com.artem.tusaandroid.socket.SocketListener
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.launch
import javax.inject.Inject

@HiltViewModel
open class ChatViewModel @Inject constructor(
    val chatsState: ChatState?,
    val friendsState: FriendsState?,
    val socketListener: SocketListener?
): ViewModel() {
    var loading = mutableStateOf(false)
    val loadSize = 30
    var page = 0
    var totalPages = 1
    var messages: MutableState<List<MessageResponse>> = mutableStateOf(listOf())
    var lazyListState: LazyListState = LazyListState()

    private val listenerMessages = object: EventListener<ResponseMessages> {
        override fun onEvent(event: ResponseMessages) {
            // получили новые сообщения
            if (event.page == page && event.chatId == getChat().value?.chatId) {
                val newMessages = event.messages
                messages.value = messages.value.filter { it.chatId != -1L }.toList() + newMessages
                page++
                totalPages = event.totalPages
                loading.value = false
            }
        }
    }

    // новое сообщение в моменте
    private val listenerNewMessage = object : EventListener<MessageResponse> {
        override fun onEvent(event: MessageResponse) {
            if (event.chatId == getChat().value?.chatId) {
                messages.value = listOf(event) + messages.value
                viewModelScope.launch {
                    lazyListState.scrollToItem(0)
                }
            }
        }
    }

    init {
        socketListener?.getReceiveMessage()?.messagesBus?.addListener(listenerMessages)
        socketListener?.getReceiveMessage()?.sendMessageNotifyBus?.addListener(listenerNewMessage)
    }

    fun getCloseChatCount(): Int {
        return chatsState?.getCloseChatCount()?: 0
    }

    fun getModalOpened() = chatsState?.modalOpened?: mutableStateOf(true)

    fun getChat() = chatsState?.chat?: mutableStateOf<ChatResponse?>(null)

    fun closeChat() {
        chatsState?.saveChatCache(
            ChatCache(
                messages = messages.value,
                page = page,
                totalPages = totalPages,
                loadSize = loadSize
            ))
        chatsState?.closeChat()
    }

    fun getChatFriend(): FriendDto? {
        return friendsState?.friends?.value?.find { it.id == getChat().value?.toId }
    }

    fun sendMessage(message: String) {
        val currentChat = chatsState?.chat?.value?: return
        val sendMessage = SendMessage(
            toId = currentChat.toId,
            message = message,
            payload = listOf()
        )
        socketListener?.getSendMessage()?.sendChatMessage(sendMessage)
        messages.value = listOf(MessageResponse(
            ownerId = chatsState.getUserId(),
            toId = currentChat.toId,
            chatId = -1,
            payload = listOf(),
            message = message,
            creation = System.currentTimeMillis(),
            deletedOwner = false,
            deletedTo = false,
            changed = false,
            read = false,
        )) + messages.value
        viewModelScope.launch {
            lazyListState.scrollToItem(0)
        }
    }

    fun loadMoreItems() {
        if (page < totalPages && loading.value == false) {
            val chatId = getChat().value?.chatId?: return
            socketListener?.getSendMessage()?.getMessages(chatId, page, loadSize)
            loading.value = true
            messages.value = messages.value.plus(
                MessageResponse(
                    ownerId = 0,
                    toId = 0,
                    chatId = -1,
                    payload = listOf(),
                    message = "",
                    creation = System.currentTimeMillis(),
                    deletedOwner = false,
                    deletedTo = false,
                    changed = false,
                    read = false,
                )
            )
        }
    }

    fun newChatOpened() {
        loading.value = false
        totalPages = 1
        page = 0
        messages.value = listOf()
        val cache = chatsState?.getChatCache()
        if (cache != null) {
            messages.value = cache.messages
            page = cache.page
            totalPages = cache.totalPages
        }

        loadMoreItems()
    }
}