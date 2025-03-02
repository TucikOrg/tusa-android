package com.artem.tusaandroid.app.image.preview

import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.hilt.navigation.compose.hiltViewModel
import com.artem.tusaandroid.app.image.preview.ui.FullScreenImageDialog

@Composable
fun ImagesPreviewDialog(model: ImagesPreviewDialogViewModel = hiltViewModel()) {
    val dialog by model.getDialogState().dialog
    if (dialog != null) {
        FullScreenImageDialog(
            model,
            images = dialog!!.images,
            initialIndex = 0,
        )
    }
}