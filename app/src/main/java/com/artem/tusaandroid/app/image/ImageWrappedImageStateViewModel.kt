package com.artem.tusaandroid.app.image

import androidx.lifecycle.ViewModel
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject


@HiltViewModel
open class ImageWrappedImageStateViewModel @Inject constructor(
    val imageState: ImageState,
): ViewModel()