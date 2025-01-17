package com.artem.tusaandroid.socket

import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf

class SocketConnectionState {
    private val state = mutableStateOf(SocketConnectionStates.CLOSED)
    val socketStateBus: EventBus<SocketConnectionStates> = EventBus()

    fun getState() = state

    fun opened() {
        state.value = SocketConnectionStates.OPEN
        socketStateBus.pushEvent(SocketConnectionStates.OPEN)
    }

    fun closed() {
        state.value = SocketConnectionStates.CLOSED
        socketStateBus.pushEvent(SocketConnectionStates.CLOSED)
    }

    fun failed() {
        state.value = SocketConnectionStates.FAILED
        socketStateBus.pushEvent(SocketConnectionStates.FAILED)
    }

    fun waitToReconnect() {
        state.value = SocketConnectionStates.WAIT_TO_RECONNECT
        socketStateBus.pushEvent(SocketConnectionStates.WAIT_TO_RECONNECT)
    }
}

fun SocketConnectionStates.isClosed(): Boolean {
    return this == SocketConnectionStates.CLOSED || this == SocketConnectionStates.WAIT_TO_RECONNECT
}