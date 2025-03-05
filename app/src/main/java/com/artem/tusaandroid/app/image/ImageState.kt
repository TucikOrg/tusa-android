package com.artem.tusaandroid.app.image

import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.graphics.drawable.AnimatedImageDrawable
import android.net.Uri
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.produceState
import androidx.core.graphics.drawable.toBitmap
import coil.ImageLoader
import coil.decode.GifDecoder
import coil.decode.ImageDecoderDecoder
import coil.request.CachePolicy
import coil.request.ImageRequest
import com.artem.tusaandroid.dto.ImageDto
import com.artem.tusaandroid.socket.EventListener
import com.artem.tusaandroid.socket.SocketListener
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.launch
import java.io.ByteArrayOutputStream

class ImageState(
    private val socketListener: SocketListener,
    private val imageDao: ImageDao,
    val tempIdToUriDao: TempIdToUriDao
) {
    private val imageBitmapsMap: MutableMap<String, MutableState<Bitmap?>> = mutableMapOf()
    private val uriMaps: MutableMap<String, Uri?> = mutableMapOf()


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

    suspend fun getImage(tempFileId: String, ownerId: Long): ImageData {
        var uri: Uri? = null
        if (uriMaps.containsKey(tempFileId)) {
            uri = uriMaps[tempFileId]
        } else {
            uri = tempIdToUriDao.findById(tempFileId)?.let {
                Uri.parse(it.uri)
            }
            uriMaps[tempFileId] = uri
        }

        val bitmapState = getImageBitmap(tempFileId, ownerId)
        return ImageData(
            uri = uri,
            bitmap = bitmapState
        )
    }

    private suspend fun getImageBitmap(tempId: String, ownerId: Long): MutableState<Bitmap?> {
        if (imageBitmapsMap[tempId] == null) {
            imageBitmapsMap[tempId] = mutableStateOf(null)
        }

        // Если уже есть в кэше — сразу вернуть
        if (imageBitmapsMap[tempId]!!.value != null) {
            return imageBitmapsMap[tempId]!!
        }

        // Загружаем изображение асинхронно
        val imageEntity = imageDao.findById(tempId)
        if (imageEntity != null) {
            imageBitmapsMap[tempId]!!.value = BitmapFactory.decodeByteArray(imageEntity.image, 0, imageEntity.image.size)
            return imageBitmapsMap[tempId]!!  // загрузили из локальной базы и выходим
        }

        // загружаем картинку/фото/изображение
        socketListener.getSendMessage()?.image(tempId, ownerId)

        return imageBitmapsMap[tempId]!!
    }

    fun getGifByUrl(url: String, context: Context, viewModelScope: CoroutineScope): MutableState<Bitmap?> {
        var imageBitmapMap = imageBitmapsMap[url]
        if (imageBitmapMap == null) {
            imageBitmapMap = mutableStateOf(null)
            imageBitmapsMap[url] = imageBitmapMap
        }

        viewModelScope.launch {
            // нету в памяти картинки
            if (imageBitmapMap.value == null) {
                // ищем в базе
                val imageEntity = imageDao.findById(url)
                if (imageEntity != null) {
                    imageBitmapsMap[url]!!.value = BitmapFactory.decodeByteArray(imageEntity.image, 0, imageEntity.image.size)
                    return@launch
                }

                // грузим из интернета потому что нету ифнормации о картинке в базе
                val requestImage = ImageRequest.Builder(context)
                    .data(url)
                    .decoderFactory(ImageDecoderDecoder.Factory())
                    .build()
                val imageLoader = ImageLoader.Builder(context).build()
                val drawable = imageLoader.execute(requestImage).drawable
                val bitmap = drawable!!.toBitmap()
                imageBitmapsMap[url]!!.value = bitmap

                ByteArrayOutputStream().use {
                    bitmap.compress(Bitmap.CompressFormat.JPEG, 100, it)
                    imageDao.insert(ImageEntity(
                        tempId = url,
                        image = it.toByteArray()
                    ))
                }
            }
        }

        return imageBitmapMap
    }
}