package com.artem.tusaandroid.socket

import com.artem.tusaandroid.app.profile.ProfileState

class SocketConnect(
    val profileState: ProfileState,
    val socketListener: SocketListener
) {
    fun safeConnectCall(reason: String) {
        if (profileState.getIsAuthenticated()) {
            socketListener.connect(reason)
        }
    }

    fun disconnect(reason: String) {
        socketListener.disconnect(reason)
    }
}