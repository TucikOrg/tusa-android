package com.artem.tusaandroid.app

import android.graphics.Bitmap
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf

class MeAvatarState {
    private var avatarBitmap = mutableStateOf<Bitmap?>(null)
    private var avatarBytes: ByteArray? = null

    private var needUpdateInRenderFlag = false

    fun getAvatar(): MutableState<Bitmap?> {
        return avatarBitmap
    }

    fun getNeedUpdateInRenderFlag(): Boolean {
        return needUpdateInRenderFlag
    }

    fun getAvatarBytes(): ByteArray? {
        return avatarBytes
    }

    fun rendererUpdated() {
        needUpdateInRenderFlag = false
    }

    fun setAvatar(avatar: Bitmap?, bytesArray: ByteArray?) {
        avatarBitmap.value = avatar
        avatarBytes = bytesArray
        needUpdateInRenderFlag = true
    }
}