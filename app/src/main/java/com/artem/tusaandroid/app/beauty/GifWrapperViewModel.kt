package com.artem.tusaandroid.app.beauty

import android.content.Context
import android.graphics.Bitmap
import androidx.compose.runtime.MutableState
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.app.image.ImageState
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
open class GifWrapperViewModel @Inject constructor(
    val imageState: ImageState
): ViewModel() {

    fun getGifByUrl(url: String, context: Context): MutableState<Bitmap?> {
        return imageState.getGifByUrl(url, context, viewModelScope)
    }
}