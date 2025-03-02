package com.artem.tusaandroid.app.chat

import android.net.Uri
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.ElevatedButton
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.produceState
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.asImageBitmap
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.unit.dp
import androidx.hilt.navigation.compose.hiltViewModel
import androidx.lifecycle.LifecycleOwner
import androidx.lifecycle.compose.LocalLifecycleOwner
import coil.compose.AsyncImage
import com.artem.tusaandroid.R
import com.artem.tusaandroid.app.beauty.ShimmerBox
import com.artem.tusaandroid.app.image.ImageData
import com.artem.tusaandroid.app.image.ImageWrappedImageState
import com.artem.tusaandroid.app.image.preview.ui.ImagePreviewInput
import com.artem.tusaandroid.dto.MessageResponse
import com.artem.tusaandroid.room.messenger.UploadingImageStatus

@Composable
fun ImageInChat(
    chatViewModel: ChatViewModel,
    tempFileId: String,
    messageResponse: MessageResponse,
    imageInChatViewModel: ImageInChatViewModel = hiltViewModel()
) {
    val withUser = chatViewModel.getWithUserId()
    Box(
        modifier = Modifier
            .fillMaxWidth()
            .height(250.dp)
            .clickable {
                imageInChatViewModel.openFullScreenImageDialog(
                    images = listOf(ImagePreviewInput(
                        tempFileId = tempFileId,
                        ownerId = withUser
                    ))
                )
            }
    ) {
        // Пробуем взять картинку
        ImageWrappedImageState(
            modifier = Modifier.fillMaxSize(),
            tempFileId = tempFileId,
            ownerId = withUser,
        )

        val imageStatus by imageInChatViewModel.getImageStatus(tempFileId)
            .collectAsState()
        val useStatus = UploadingImageStatus.entries[imageStatus?.status ?: UploadingImageStatus.NONE.ordinal]

        if (useStatus == UploadingImageStatus.UPLOADING) {
            Box(
                modifier = Modifier
                    .padding(vertical = 4.dp, horizontal = 6.dp)
                    .background(
                        MaterialTheme.colorScheme.background.copy(alpha = 0.9f),
                        shape = RoundedCornerShape(30)
                    )
                    .align(Alignment.BottomEnd)
            ) {
                Text(
                    text = "загружаем на сервер...",
                    modifier = Modifier.padding(4.dp)
                )
            }
        } else if (useStatus == UploadingImageStatus.ERROR) {
            val context = LocalContext.current
            val lifecycleOwner = LocalLifecycleOwner.current
            ElevatedButton(
                modifier = Modifier
                    .padding(vertical = 4.dp, horizontal = 6.dp)
                    .align(Alignment.BottomEnd),
                onClick = {
                    imageInChatViewModel.retryUploadImage(
                        chatViewModel,
                        tempFileId,
                        messageResponse,
                        context,
                        lifecycleOwner
                    )
                }
            ) {
                Text(
                    text = "Повторить",
                    modifier = Modifier.padding(horizontal = 3.dp, vertical = 2.dp),
                    style = MaterialTheme.typography.bodySmall
                )
                Icon(
                    painter = painterResource(R.drawable.refresh),
                    contentDescription = "try upload image again"
                )
            }
        }

    }
}