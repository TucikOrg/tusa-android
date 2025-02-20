package com.artem.tusaandroid.app.image

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import com.artem.tusaandroid.dto.ImageDto
import com.artem.tusaandroid.socket.EventListener
import com.artem.tusaandroid.socket.SocketListener
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.launch

class ImageState(
    private val socketListener: SocketListener,
    private val imageDao: ImageDao
) {
    private val imageBitmapsMap: MutableMap<String, MutableState<Bitmap?>> = mutableMapOf()


    fun initListener(viewModelScope: CoroutineScope) {
        socketListener.getReceiveMessage().imageBus.addListener(object: EventListener<ImageDto> {
            override fun onEvent(event: ImageDto) {
                if (event.image == null) {
                    return
                }

                if (imageBitmapsMap[event.localFilePathId] == null) {
                    imageBitmapsMap[event.localFilePathId] = mutableStateOf(null)
                }
                imageBitmapsMap[event.localFilePathId]!!.value = BitmapFactory.decodeByteArray(event.image, 0, event.image.size)

                viewModelScope.launch {
                    imageDao.insert(ImageEntity(
                        tempId = event.localFilePathId,
                        image = event.image
                    ))
                }
            }
        })
    }

    fun getImageBitmap(tempId: String, ownerId: Long, viewModelScope: CoroutineScope): MutableState<Bitmap?> {
        if (imageBitmapsMap[tempId] == null) {
            imageBitmapsMap[tempId] = mutableStateOf(null)
        }

        // Если уже есть в кэше — сразу вернуть
        if (imageBitmapsMap[tempId]!!.value != null) {
            return imageBitmapsMap[tempId]!!
        }

        // Загружаем изображение асинхронно
        viewModelScope.launch {
            val imageEntity = imageDao.findById(tempId)
            if (imageEntity != null) {
                imageBitmapsMap[tempId]!!.value = BitmapFactory.decodeByteArray(imageEntity.image, 0, imageEntity.image.size)
                return@launch // загрузили из локальной базы и выходим
            }

            // загружаем картинку/фото/изображение
            socketListener.getSendMessage()?.image(tempId, ownerId)
        }

        return imageBitmapsMap[tempId]!!
    }
}