package com.artem.tusaandroid.app

import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import com.artem.tusaandroid.dto.IsOnlineDto
import com.artem.tusaandroid.socket.EventListener
import com.artem.tusaandroid.socket.SocketListener

class IsOnlineState(
    private val socketListener: SocketListener
) {
    init {
        socketListener.getReceiveMessage().isOnlineBus.addListener(object : EventListener<IsOnlineDto> {
            override fun onEvent(event: IsOnlineDto) {
                if (usersOnlineMap[event.userId] == null) {
                    usersOnlineMap[event.userId] = mutableStateOf(event.isOnline)
                }

                usersOnlineMap[event.userId]!!.value = event.isOnline
            }
        })
    }

    private var usersOnlineMap: MutableMap<Long, MutableState<Boolean>> = mutableMapOf()

    fun isUserOnline(userId: Long): MutableState<Boolean> {
        if (usersOnlineMap[userId] == null) {
            usersOnlineMap[userId] = mutableStateOf(false)
        }

        return usersOnlineMap[userId]!!
    }
}