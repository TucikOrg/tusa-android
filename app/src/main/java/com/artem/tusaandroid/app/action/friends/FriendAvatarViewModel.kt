package com.artem.tusaandroid.app.action.friends

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.app.avatar.AvatarDTO
import com.artem.tusaandroid.socket.EventListener
import com.artem.tusaandroid.socket.ReceiveMessage
import com.artem.tusaandroid.socket.SendMessage
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
open class FriendAvatarViewModel @Inject constructor(
    private val sendMessage: SendMessage?,
    private val receiveMessage: ReceiveMessage?
): ViewModel() {
    var waitAvatarId = -1L

    init {
        receiveMessage?.avatarBus?.addListener(object: EventListener<AvatarDTO> {
            override fun onEvent(event: AvatarDTO) {
                if (event.ownerId != waitAvatarId) {
                    return
                }
                val bitmap = BitmapFactory.decodeByteArray(event.avatar, 0, event.avatar.size)
                avatarBitmap.value = bitmap
            }
        })
    }

    val avatarBitmap: MutableState<Bitmap?> = mutableStateOf(null)

    fun loadAvatar(userId: Long) {
        waitAvatarId = userId
        sendMessage?.loadAvatar(userId)
    }
}
