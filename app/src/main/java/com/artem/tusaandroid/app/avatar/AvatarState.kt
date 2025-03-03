package com.artem.tusaandroid.app.avatar

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import com.artem.tusaandroid.dto.AvatarDTO
import com.artem.tusaandroid.location.LocationsState
import com.artem.tusaandroid.room.AvatarDao
import com.artem.tusaandroid.room.AvatarRoomEntity
import com.artem.tusaandroid.socket.EventBus
import com.artem.tusaandroid.socket.EventListener
import com.artem.tusaandroid.socket.SocketConnectionState
import com.artem.tusaandroid.socket.SocketConnectionStates
import com.artem.tusaandroid.socket.SocketListener
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.launch


class AvatarState(
    private val socketListener: SocketListener?,
    private val avatarDao: AvatarDao?,
    private val socketConnectionState: SocketConnectionState?,
    private val locationsState: LocationsState?,
    private val assetManager: android.content.res.AssetManager?
) {
    private val bitmaps: MutableMap<Long, MutableState<Bitmap?>> = mutableMapOf()
    private val bytes: MutableMap<Long, ByteArray> = mutableMapOf()
    private val loadingAvatars: MutableMap<Long, MutableState<Boolean>> = mutableMapOf()
    private var defaultAvatar: ByteArray? = null

    val avatarEventBus: EventBus<AvatarUI> = EventBus()

    fun getDefaultAvatar(): ByteArray {
        return defaultAvatar!!
    }

    fun initListeners(viewModelScope: CoroutineScope) {
        socketConnectionState?.socketStateBus?.addListener(object: EventListener<SocketConnectionStates> {
            override fun onEvent(event: SocketConnectionStates) {
                if (event == SocketConnectionStates.OPEN) {
                    // если интернет соединение оборвалось то заново пытаемся получить запросы по автаркам
                    for (avatarId in loadingAvatars.keys) {
                        loadAvatar(avatarId)
                    }
                }
            }
        })

        // аватрака была загружена
        socketListener?.getReceiveMessage()?.avatarBus?.addListener(object: EventListener<AvatarDTO> {
            override fun onEvent(event: AvatarDTO) {
                loadingAvatars[event.ownerId]?.value = false
                var useAvatar = defaultAvatar!! // если авы нету то используем дефолтную
                if (event.avatar != null) {
                    // если мы загрузили автарку какую-то то используем ее
                    useAvatar = event.avatar
                }

                val newBitmap = BitmapFactory.decodeByteArray(useAvatar, 0, useAvatar.size)
                val bitmap = bitmaps[event.ownerId]
                if (bitmap == null) {
                    bitmaps[event.ownerId] = mutableStateOf(newBitmap)
                } else {
                    bitmaps[event.ownerId]?.value = newBitmap
                }

                bytes[event.ownerId] = useAvatar
                viewModelScope.launch {
                    avatarDao?.insert(AvatarRoomEntity(
                        id = event.ownerId,
                        avatar = useAvatar,
                        updatingTime = event.updatingTime
                    ))
                }

                avatarEventBus.pushEvent(AvatarUI(
                    id = event.ownerId,
                    bitmap = bitmaps[event.ownerId]!!,
                    byteArray = useAvatar,
                    isNetwork = true
                ))
            }
        })
    }

    init {
        assetManager?.open("images/default_user.png").use { asset ->
            defaultAvatar = asset?.readBytes()
        }
    }

    fun retrieveAvatar(userId: Long, scope: CoroutineScope, forceReload: Boolean = false){
        if (existsInMemory(userId) && !forceReload) {
            val result = AvatarUI(userId, bitmaps[userId]!!, bytes[userId]!!, false)
            avatarEventBus.pushEvent(result)
            return
        }

        // пробуем взять аватар из локальной базы
        val avatarEntity = avatarDao?.getAvatarById(userId)
        if (avatarEntity != null && !forceReload) {
            avatarEntity.avatar?.let {
                val bitmap = BitmapFactory.decodeByteArray(it, 0, it.size)
                saveAvatarInMem(userId, bitmap, it)
                val result = AvatarUI(userId, bitmaps[userId]!!, bytes[userId]!!, false)
                avatarEventBus.pushEvent(result)
                return
            }
        }

        // если нету в локальной базе то загружаем из сети
        loadAvatar(userId)
        return
    }

    fun isLoading(userId: Long): MutableState<Boolean> {
        if (loadingAvatars[userId] == null) {
            loadingAvatars[userId] = mutableStateOf(false)
        }
        return loadingAvatars[userId]!!
    }

    fun getAvatarBitmap(userId: Long): MutableState<Bitmap?> {
        if (bitmaps[userId] == null) {
            bitmaps[userId] = mutableStateOf(null)
        }
        return bitmaps[userId]!!
    }

    fun getAvatarBytes(userId: Long): ByteArray? {
        return bytes[userId]
    }


    private fun existsInMemory(userId: Long): Boolean {
        return bitmaps[userId] != null && bitmaps[userId]?.value != null
    }

    private fun loadAvatar(userId: Long) {
        if (loadingAvatars[userId] == null) {
            loadingAvatars[userId] = mutableStateOf(true)
        } else {
            loadingAvatars[userId]!!.value = true
        }

        socketListener?.getSendMessage()?.loadAvatar(userId)
    }

    fun saveAvatarInMem(userId: Long, avatar: Bitmap, bytesArray: ByteArray) {
        if (bitmaps[userId] == null) {
            bitmaps[userId] = mutableStateOf(avatar)
        } else {
            bitmaps[userId]?.value = avatar
        }

        bytes[userId] = bytesArray
    }

    fun clear(userId: Long) {
        bitmaps[userId]?.value = null
        bytes.remove(userId)
    }
}