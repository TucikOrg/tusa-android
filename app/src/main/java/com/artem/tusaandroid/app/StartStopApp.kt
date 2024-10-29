package com.artem.tusaandroid.app

import com.artem.tusaandroid.app.action.friends.FriendsState
import com.artem.tusaandroid.socket.WebSocketClient

class StartStopApp(
    private val meAvatarState: MeAvatarState?,
    private val friendsState: FriendsState?,
    private val webSocketClient: WebSocketClient?
) {
    fun onLogout() {
        webSocketClient?.disconnect()
    }

    fun onLogin() {
        webSocketClient?.connect()

        meAvatarState?.loadMeAvatar()

        friendsState?.loadFriendsAndRequests()
    }
}
