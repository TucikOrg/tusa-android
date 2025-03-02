package com.artem.tusaandroid.app.chat

import android.content.Context
import android.graphics.Bitmap
import android.net.Uri
import androidx.compose.runtime.MutableState
import androidx.lifecycle.LifecycleOwner
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.app.image.ImageData
import com.artem.tusaandroid.app.image.ImageState
import com.artem.tusaandroid.app.image.TempIdToUriDao
import com.artem.tusaandroid.app.image.preview.ImagesPreviewDialogState
import com.artem.tusaandroid.app.image.preview.ui.ImagePreviewInput
import com.artem.tusaandroid.dto.MessageResponse
import com.artem.tusaandroid.dto.SendMessage
import com.artem.tusaandroid.room.messenger.ImageUploadingStatusDao
import com.artem.tusaandroid.room.messenger.ImageUploadingStatusEntity
import com.artem.tusaandroid.room.messenger.UploadingImageStatus
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.SharingStarted
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.stateIn
import kotlinx.coroutines.launch
import javax.inject.Inject


@HiltViewModel
open class ImageInChatViewModel @Inject constructor(
    val tempIdToUriDao: TempIdToUriDao,
    val imageState: ImageState,
    val imageUploadingStatusDao: ImageUploadingStatusDao,
    val imagesPreviewDialogState: ImagesPreviewDialogState
): ViewModel() {
    private val imageUploadingStatesMap = mutableMapOf<String, StateFlow<ImageUploadingStatusEntity?>>()

    suspend fun getUriByTempId(tempId: String): Uri? {
        tempIdToUriDao.findById(tempId)?.let {
            return Uri.parse(it.uri)
        }

        return null
    }

    suspend fun getImageData(tempFileId: String, ownerId: Long): ImageData {
        return imageState.getImage(tempFileId, ownerId)
    }

    fun getImageStatus(tempFileId: String): StateFlow<ImageUploadingStatusEntity?> {
        return imageUploadingStatesMap.getOrPut(tempFileId) {
            imageUploadingStatusDao.findStatus(tempFileId).stateIn(
                scope = viewModelScope,
                started = SharingStarted.WhileSubscribed(5000),
                initialValue = null
            )
        }
    }

    override fun onCleared() {
        imageUploadingStatesMap.clear()
        super.onCleared()
    }

    fun retryUploadImage(
        chatViewModel: ChatViewModel,
        tempId: String,
        message: MessageResponse, // чтобы переслать сообщение при наличии всех подгруженных картинок
        context: Context,
        lifecycleOwner: LifecycleOwner
    ) {
        viewModelScope.launch {
            val uri = getUriByTempId(tempId)
            if (uri == null) return@launch
            chatViewModel.uploadImageToServer(
                uri = uri,
                fileId = tempId,
                sendMessage = SendMessage(
                    toId = chatViewModel.getWithUserId(),
                    message = message.message,
                    temporaryId = message.temporaryId,
                    payload = message.getClearedPayload()
                ),
                context = context,
                lifecycleOwner = lifecycleOwner
            )
        }

    }

    fun openFullScreenImageDialog(images: List<ImagePreviewInput>) {
        imagesPreviewDialogState.open(
            title = "",
            images = images
        )
    }
}