package com.artem.tusaandroid.app.image.preview

import androidx.lifecycle.ViewModel
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
open class ImagesPreviewDialogViewModel @Inject constructor(
    private val imagesPreviewDialogState: ImagesPreviewDialogState,
): ViewModel() {

    fun getDialogState() = imagesPreviewDialogState
    fun close() {
        imagesPreviewDialogState.close()
    }
}