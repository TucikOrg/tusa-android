package com.artem.tusaandroid.app.image.preview

import androidx.compose.runtime.mutableStateOf
import com.artem.tusaandroid.app.image.preview.ui.ImagePreviewInput

class ImagesPreviewDialogState {
    val dialog = mutableStateOf<ImagesPreviewDialogData?>(null)

    fun open(title: String, images: List<ImagePreviewInput>) {
        dialog.value = ImagesPreviewDialogData(
            title = title,
            images = images
        )
    }

    fun close() {
        dialog.value = null
    }
}