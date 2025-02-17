package com.artem.tusaandroid.firebase

import com.artem.tusaandroid.socket.SocketListener

class FirebaseState(
    private val socketListener: SocketListener
) {
    private var token: String? = null

    fun saveToken(token: String) {
        // сохраняем пользовательский токен firebase на сервере
        this.token = token
        socketListener.getSendMessage()?.sendFirebaseToken(token)
    }

    fun resaveSame() {
        token?.let {
            socketListener.getSendMessage()?.sendFirebaseToken(it)
        }
    }
}