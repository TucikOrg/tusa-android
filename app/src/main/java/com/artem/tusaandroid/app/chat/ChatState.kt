package com.artem.tusaandroid.app.chat

import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.dto.ChatResponse
import com.artem.tusaandroid.socket.EventListener
import com.artem.tusaandroid.socket.SocketListener
import kotlin.Long

class ChatState(
    private val socketListener: SocketListener,
    private val profileState: ProfileState
) {
    private var closeChatCount = 0
    private var waitChatWith = 0L
    var chat: MutableState<ChatResponse?> = mutableStateOf(null)

    private val chatsCache: MutableMap<Long, ChatCache> = mutableMapOf()
    fun getChatCache(): ChatCache? {
        if (chat.value == null) {
            return null
        } else {
            return chatsCache[chat.value!!.chatId]
        }
    }
    fun saveChatCache(chatCache: ChatCache) {
        if (chat.value != null) {
            chatsCache[chat.value!!.chatId] = chatCache
        }
    }

    private val listener = object: EventListener<ChatResponse> {
        override fun onEvent(event: ChatResponse) {
            if (event.toId == waitChatWith) {
                waitChatWith = 0

                // дождались чат который хотим открыть
                openChat(event)
            }
        }
    }

    private val noChatListener = object: EventListener<Long> {
        override fun onEvent(userId: Long) {
            if (userId == waitChatWith) {
                openChat(ChatResponse(
                    chatId = -1,
                    ownerId = profileState.getUserId(),
                    toId = userId,
                    muted = false,
                    lastMessage = "",
                    lastMessageOwner = -1
                ))
            }
        }
    }

    init {
        socketListener.getReceiveMessage().findChatBus.addListener(listener)
        socketListener.getReceiveMessage().noChatBus.addListener(noChatListener)
    }

    val modalOpened = mutableStateOf(false)

    fun getUserId(): Long {
        return profileState.getUserId()
    }

    fun getCloseChatCount(): Int {
        return closeChatCount
    }

    fun closeChat() {
        modalOpened.value = false
        chat.value = null
        closeChatCount++
    }

    fun openChatWithUser(userId: Long) {
        waitChatWith = userId
        socketListener.getSendMessage()?.findChatWithUser(userId)
    }

    fun openChat(chat: ChatResponse) {
        this.chat.value = chat
        modalOpened.value = true
    }
}


