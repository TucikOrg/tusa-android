package com.artem.tusaandroid.app.profile

import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.app.IsOnlineState
import com.artem.tusaandroid.socket.SocketConnectionState
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
open class ProfileUserColumnViewModel @Inject constructor(
    private val isOnlineState: IsOnlineState,
): ViewModel() {
    fun isOnlineState(id: Long) = isOnlineState.isUserOnline(id)
}