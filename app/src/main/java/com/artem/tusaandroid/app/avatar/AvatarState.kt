package com.artem.tusaandroid.app.avatar

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import com.artem.tusaandroid.socket.EventListener
import com.artem.tusaandroid.socket.SocketListener


class AvatarState(
    private val socketListener: SocketListener?
) {
    private val bitmaps: MutableMap<Long, MutableState<Bitmap?>> = mutableMapOf()
    private val bytes: MutableMap<Long, ByteArray> = mutableMapOf()

    fun getAvatarBitmap(userId: Long): MutableState<Bitmap?> {
        if (bitmaps[userId] == null) {
            bitmaps[userId] = mutableStateOf(null)
        }
        return bitmaps[userId]!!
    }

    fun getAvatarBytes(userId: Long): ByteArray? {
        return bytes[userId]
    }

    init {
        socketListener?.getReceiveMessage()?.avatarBus?.addListener(object: EventListener<AvatarDTO> {
            override fun onEvent(event: AvatarDTO) {

                val newBitmap = BitmapFactory.decodeByteArray(event.avatar, 0, event.avatar.size)
                val bitmap = bitmaps[event.ownerId]
                if (bitmap == null) {
                    bitmaps[event.ownerId] = mutableStateOf(newBitmap)
                } else {
                    bitmaps[event.ownerId]?.value = newBitmap
                }

                bytes[event.ownerId] = event.avatar
            }
        })
    }

    fun loadAvatar(userId: Long) {
        if (bitmaps[userId]?.value != null) {
            return
        }

        socketListener?.getSendMessage()?.loadAvatar(userId)
    }

    fun setAvatarBitmap(userId: Long, avatar: Bitmap) {
        if (bitmaps[userId] == null) {
            bitmaps[userId] = mutableStateOf(avatar)
        } else {
            bitmaps[userId]?.value = avatar
        }
    }

    fun setAvatarBytes(userId: Long, bytesArray: ByteArray) {
        bytes[userId] = bytesArray
    }

    fun clear(userId: Long) {
        bitmaps[userId]?.value = null
        bytes.remove(userId)
    }
}