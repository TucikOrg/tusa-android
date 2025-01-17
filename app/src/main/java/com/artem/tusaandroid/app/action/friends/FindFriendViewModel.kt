package com.artem.tusaandroid.app.action.friends

import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.dto.FriendDto
import com.artem.tusaandroid.socket.EventListener
import com.artem.tusaandroid.socket.ReceiveMessage
import com.artem.tusaandroid.socket.SendMessage
import com.artem.tusaandroid.socket.SocketBinaryMessage
import com.artem.tusaandroid.socket.SocketListener
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.serialization.ExperimentalSerializationApi
import kotlinx.serialization.cbor.Cbor
import kotlinx.serialization.encodeToByteArray
import javax.inject.Inject

@HiltViewModel
open class FindFriendViewModel @Inject constructor(
    private val socketListener: SocketListener?,
): ViewModel() {

    init {
        socketListener?.getReceiveMessage()?.findUsersBus?.addListener(object : EventListener<List<FriendDto>> {
            override fun onEvent(event: List<FriendDto>) {
                users.value = event
            }
        })
    }

    val showModal: MutableState<Boolean> = mutableStateOf(false)
    val searchQuery: MutableState<String> = mutableStateOf("")
    val users: MutableState<List<FriendDto>> = mutableStateOf(listOf())

    @OptIn(ExperimentalSerializationApi::class)
    fun findUser() {
        if (searchQuery.value.isEmpty()) {
            users.value = listOf()
            return
        }
        socketListener?.getSendMessage()?.sendMessage(SocketBinaryMessage("find-users", Cbor.encodeToByteArray(searchQuery.value)))
    }
}