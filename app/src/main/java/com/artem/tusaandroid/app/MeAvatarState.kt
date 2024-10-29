package com.artem.tusaandroid.app

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import com.artem.tusaandroid.app.avatar.AvatarDTO
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.socket.EventListener
import com.artem.tusaandroid.socket.ReceiveMessage
import com.artem.tusaandroid.socket.SendMessage

class MeAvatarState(
    private val profileState: ProfileState?,
    private val sendMessage: SendMessage?,
    private val receiveMessage: ReceiveMessage?
) {
    private val meAvatarEvent = object: EventListener<AvatarDTO> {
        override fun onEvent(event: AvatarDTO) {
            if (event.ownerId != profileState?.getUserId()) {
                return
            }
            val bitmap = BitmapFactory.decodeByteArray(event.avatar, 0, event.avatar.size)
            val bytes = event.avatar
            setAvatar(bitmap, bytes)
        }
    }

    init {
        receiveMessage?.avatarBus?.addListener(meAvatarEvent)
    }

    private var avatarBitmap = mutableStateOf<Bitmap?>(null)
    private var avatarBytes: ByteArray? = null
    private var needUpdateInRenderFlag = false
    private var hideMeFlag = false

    fun getAvatar(): MutableState<Bitmap?> {
        return avatarBitmap
    }

    fun getNeedUpdateInRenderFlag(): Boolean {
        return needUpdateInRenderFlag
    }

    fun getHideMeFlag(): Boolean {
        return hideMeFlag
    }

    fun getAvatarBytes(): ByteArray? {
        return avatarBytes
    }

    fun rendererUpdated() {
        needUpdateInRenderFlag = false
    }

    fun updateMeMarkerInRender() {
        needUpdateInRenderFlag = true
    }

    fun hideMe() {
        hideMeFlag = true
    }

    fun iAmHidden() {
        hideMeFlag = false
    }

    fun setAvatar(avatar: Bitmap?, bytesArray: ByteArray?) {
        avatarBitmap.value = avatar
        avatarBytes = bytesArray
        needUpdateInRenderFlag = true
    }

    fun clearAvatar() {
        avatarBitmap.value = null
        avatarBytes = null
    }

    fun loadMeAvatar() {
        profileState?.getUserId().let {
            sendMessage?.loadAvatar(it!!)
        }
    }
}