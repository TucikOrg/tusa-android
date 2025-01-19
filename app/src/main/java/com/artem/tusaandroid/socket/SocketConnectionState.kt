package com.artem.tusaandroid.socket

import androidx.compose.runtime.mutableStateOf

class SocketConnectionState {
    private val state = mutableStateOf(SocketConnectionStates.CLOSED)
    private var closeStatus: Int = -1
    val socketStateBus: EventBus<SocketConnectionStates> = EventBus()

    fun getState() = state
    fun getCloseStatus() = closeStatus

    fun opened() {
        state.value = SocketConnectionStates.OPEN
        socketStateBus.pushEvent(SocketConnectionStates.OPEN)
    }

    fun closed(status: Int) {
        closeStatus = status
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

    fun isOpened(): Boolean {
        return state.value == SocketConnectionStates.OPEN
    }

    fun isConnecting(): Boolean {
        return state.value == SocketConnectionStates.CONNECTING
    }

    fun connecting() {
        state.value == SocketConnectionStates.CONNECTING
        socketStateBus.pushEvent(SocketConnectionStates.CONNECTING)
    }

    fun isWaitToReconnect(): Boolean {
        return state.value == SocketConnectionStates.WAIT_TO_RECONNECT
    }
}

fun SocketConnectionStates.isClosed(): Boolean {
    return this == SocketConnectionStates.CLOSED || this == SocketConnectionStates.WAIT_TO_RECONNECT
}