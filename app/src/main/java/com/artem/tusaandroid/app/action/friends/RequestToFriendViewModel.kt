package com.artem.tusaandroid.app.action.friends

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.R
import com.artem.tusaandroid.socket.SocketBinaryMessage
import com.artem.tusaandroid.socket.SocketListener
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import kotlinx.serialization.ExperimentalSerializationApi
import kotlinx.serialization.cbor.Cbor
import kotlinx.serialization.encodeToByteArray
import javax.inject.Inject

@HiltViewModel
open class RequestToFriendViewModel @Inject constructor(
    private val socketListener: SocketListener?,
    private val friendsState: FriendsState?
): ViewModel() {
    @OptIn(ExperimentalSerializationApi::class)
    fun acceptRequest(id: Long) {
        socketListener?.getSendMessage()?.acceptFriend(id)
    }

    fun removeRequestToFriend(id: Long) {
        friendsState?.removeRequestToFriend(id)
    }
}