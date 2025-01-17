package com.artem.tusaandroid.location

import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.socket.EventListener
import com.artem.tusaandroid.socket.SocketConnectionState
import com.artem.tusaandroid.socket.SocketConnectionStates
import com.artem.tusaandroid.socket.SocketListener
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
open class LoadAllFriendsLocationsViewModel @Inject constructor(
    private val socketListener: SocketListener,
    private val connectionState: SocketConnectionState
) : ViewModel() {

    init {
        // когда соединение открывается то загружаем все локации снова
        connectionState.socketStateBus.addListener(object: EventListener<SocketConnectionStates> {
            override fun onEvent(event: SocketConnectionStates) {
                if (event == SocketConnectionStates.OPEN) {
                    load()
                }
            }
        })
    }

    private fun load() {
        socketListener.getSendMessage()?.locations()
    }
}