package com.artem.tusaandroid.socket

import androidx.compose.runtime.mutableStateOf

class SocketConnectionState {
    val state = mutableStateOf(SocketConnectionStates.CLOSED)

    fun closed() {
        state.value = SocketConnectionStates.CLOSED
    }

    fun failed() {
        state.value = SocketConnectionStates.FAILED
    }

    fun waitToReconnect() {
        state.value = SocketConnectionStates.WAIT_TO_RECONNECT
    }
}