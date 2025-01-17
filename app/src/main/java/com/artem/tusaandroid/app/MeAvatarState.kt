package com.artem.tusaandroid.app

import android.graphics.Bitmap
import androidx.compose.runtime.MutableState
import com.artem.tusaandroid.app.avatar.AvatarState
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.room.AvatarDao
import com.artem.tusaandroid.room.AvatarRoomEntity

class MeAvatarState(
    private val profileState: ProfileState?,
    val avatarState: AvatarState?,
    val avatarDao: AvatarDao?
) {
    private var needUpdateInRenderFlag = false
    private var hideMeFlag = false

    fun getMeId(): Long {
        return profileState?.getUserId()!!
    }

    fun hasAvatar(): Boolean {
        return avatarState?.getAvatarBitmap(profileState!!.getUserId())?.value != null
    }

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
        // Заменяем в локальной базе устройства мою аватарку
        avatarDao?.insert(AvatarRoomEntity(
            id = getMeId(),
            avatar = bytesArray
        ))
        avatarState?.saveAvatarInMem(profileState!!.getUserId(), avatar!!, bytesArray!!)
        needUpdateInRenderFlag = true
    }

    fun clearAvatar() {
        avatarState?.clear(profileState!!.getUserId())
    }
}