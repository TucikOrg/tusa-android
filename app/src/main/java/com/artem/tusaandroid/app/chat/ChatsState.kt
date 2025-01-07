package com.artem.tusaandroid.app.chat

import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import com.artem.tusaandroid.dto.ChatResponse
import com.artem.tusaandroid.dto.ChatsResponse
import com.artem.tusaandroid.socket.EventListener
import com.artem.tusaandroid.socket.SocketListener

class ChatsState(
    private var socketListener: SocketListener?
) {
    var page = 0
    var size = 20
    var waitPage = 0

    private var listener: EventListener<ChatsResponse> = object : EventListener<ChatsResponse> {
        override fun onEvent(event: ChatsResponse) {
            if (event.page == waitPage) {
                page = event.page + 1
                chats.value = chats.value + event.chats
                waitPage = -1
            }
        }
    }

    init {
        socketListener?.getReceiveMessage()?.chatsBus?.addListener(listener)
    }

    fun loadChats() {
        waitPage = page
        socketListener?.getSendMessage()?.loadChats(page, size)
    }

    var chats: MutableState<List<ChatResponse>> = mutableStateOf(emptyList())
}