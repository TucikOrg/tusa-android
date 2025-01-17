package com.artem.tusaandroid.socket

import androidx.compose.runtime.mutableStateOf
import androidx.lifecycle.ViewModel
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
open class ConnectionStatusViewModel @Inject constructor(
    private val socketConnectionState: SocketConnectionState?
): ViewModel() {

    fun getState() = socketConnectionState?.getState()?: mutableStateOf(SocketConnectionStates.OPEN)
}