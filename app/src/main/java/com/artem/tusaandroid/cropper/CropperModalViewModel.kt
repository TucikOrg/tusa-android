package com.artem.tusaandroid.cropper

import androidx.lifecycle.ViewModel
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
open class CropperModalViewModel @Inject constructor(
    private val cropperState: CropperState
) : ViewModel() {
    fun getBitmap() = cropperState.getBitmap()
    fun getShowState() = cropperState.cropperOpened
    fun getCropperState() = cropperState
}