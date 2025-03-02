package com.artem.tusaandroid.app.image.preview.ui

import androidx.compose.foundation.ExperimentalFoundationApi
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.interaction.MutableInteractionSource
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.pager.HorizontalPager
import androidx.compose.foundation.pager.rememberPagerState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.runtime.Composable
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.unit.dp
import androidx.compose.ui.window.Dialog
import androidx.compose.ui.window.DialogProperties
import com.artem.tusaandroid.app.image.ImageWrappedImageState
import com.artem.tusaandroid.app.image.preview.ImagesPreviewDialogViewModel

@Composable
fun FullScreenImageDialog(
    model: ImagesPreviewDialogViewModel,
    images: List<ImagePreviewInput>,
    initialIndex: Int = 0,
) {
    Dialog(
        onDismissRequest = {
            model.close()
        },
        properties = DialogProperties(
            dismissOnBackPress = true,
            dismissOnClickOutside = true,
            usePlatformDefaultWidth = false
        ),
    ) {
        Box(
            modifier = Modifier
                .fillMaxSize()
                .background(Color.Black)
                .clickable(
                    interactionSource = remember { MutableInteractionSource() },
                    indication = null // Отключаем анимацию
                ) {
                    model.close()
                }
        ) {
            ImagePager(
                images = images,
                initialIndex = initialIndex
            )
        }
    }
}

@OptIn(ExperimentalFoundationApi::class)
@Composable
fun ImagePager(
    images: List<ImagePreviewInput>,
    initialIndex: Int
) {
    val pagerState = rememberPagerState(initialPage = initialIndex) {
        images.size
    }

    Column(
        modifier = Modifier.fillMaxSize(),
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        // Полноэкранный пейджер с картинками
        HorizontalPager(
            state = pagerState,
            modifier = Modifier
                .weight(1f)
                .fillMaxWidth()
        ) { page ->
            ImageWrappedImageState(
                modifier = Modifier.fillMaxSize(),
                tempFileId = images[page].tempFileId,
                ownerId = images[page].ownerId,
                contentScale = ContentScale.Fit
            )
        }

        // Индикатор точек
        Row(
            modifier = Modifier
                .padding(bottom = 16.dp),
            horizontalArrangement = Arrangement.Center
        ) {
            repeat(images.size) { index ->
                val isSelected = index == pagerState.currentPage
                Box(
                    modifier = Modifier
                        .padding(horizontal = 4.dp)
                        .size(if (isSelected) 10.dp else 8.dp)
                        .clip(CircleShape)
                        .background(if (isSelected) Color.White else Color.Gray)
                )
            }
        }
    }
}

