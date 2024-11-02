package com.artem.tusaandroid.app

import android.graphics.Bitmap
import androidx.compose.runtime.MutableState
import com.artem.tusaandroid.app.avatar.AvatarState
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.socket.SocketListener

class MeAvatarState(
    private val profileState: ProfileState?,
    private val socketListener: SocketListener?,
    private val avatarState: AvatarState?
) {
//    private val meAvatarEvent = object: EventListener<AvatarDTO> {
//        override fun onEvent(event: AvatarDTO) {
//            if (event.ownerId != profileState?.getUserId()) {
//                return
//            }
//            val bitmap = BitmapFactory.decodeByteArray(event.avatar, 0, event.avatar.size)
//            val bytes = event.avatar
//            setAvatar(bitmap, bytes)
//        }
//    }
//
//    init {
//        socketListener?.getReceiveMessage()?.avatarBus?.addListener(meAvatarEvent)
//    }

    private var needUpdateInRenderFlag = false
    private var hideMeFlag = false

    fun getAvatar(): MutableState<Bitmap?>? {
        return avatarState?.getAvatarBitmap(profileState!!.getUserId())
    }

    fun getNeedUpdateInRenderFlag(): Boolean {
        return needUpdateInRenderFlag
    }

    fun getHideMeFlag(): Boolean {
        return hideMeFlag
    }

    fun getAvatarBytes(): ByteArray? {
        return avatarState?.getAvatarBytes(profileState!!.getUserId())
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
        avatarState?.setAvatarBitmap(profileState!!.getUserId(), avatar!!)
        avatarState?.setAvatarBytes(profileState!!.getUserId(), bytesArray!!)
        needUpdateInRenderFlag = true
    }

    fun clearAvatar() {
        avatarState?.clear(profileState!!.getUserId())
    }
}