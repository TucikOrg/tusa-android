package com.artem.tusaandroid.socket

enum class SocketConnectionStates {
    OPEN,
    CLOSED,
    FAILED,
    WAIT_TO_RECONNECT
}