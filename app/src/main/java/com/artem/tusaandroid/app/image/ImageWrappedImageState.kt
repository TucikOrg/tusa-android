package com.artem.tusaandroid.app.image

import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.produceState
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.asImageBitmap
import androidx.compose.ui.layout.ContentScale
import androidx.hilt.navigation.compose.hiltViewModel
import coil.compose.AsyncImage
import com.artem.tusaandroid.app.beauty.ShimmerBox

@Composable
fun ImageWrappedImageState(
    modifier: Modifier = Modifier,
    tempFileId: String,
    ownerId: Long,
    contentScale: ContentScale = ContentScale.Crop,
    imageWrappedImageState: ImageWrappedImageStateViewModel = hiltViewModel()
) {
    val imageData by produceState<ImageData?>(initialValue = null, tempFileId, ownerId) {
        value = imageWrappedImageState.imageState.getImage(tempFileId, ownerId)
    }

    if ( imageData != null && imageData!!.uri != null) {
        AsyncImage(
            model = imageData!!.uri,
            contentDescription = "Image",
            contentScale = contentScale,
            modifier = modifier,
        )
    } else if ( imageData != null && imageData!!.bitmap.value != null) {
        androidx.compose.foundation.Image(
            bitmap = imageData!!.bitmap.value!!.asImageBitmap(),
            modifier = modifier,
            contentScale = contentScale,
            contentDescription = "User avatar",
        )
    } else {
        ShimmerBox(modifier = modifier)
    }
}