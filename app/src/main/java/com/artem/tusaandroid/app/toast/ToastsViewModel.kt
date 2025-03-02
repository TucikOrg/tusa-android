package com.artem.tusaandroid.app.toast

import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.app.image.ImageState
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject


@HiltViewModel
open class ToastsViewModel @Inject constructor(
    val imageState: ImageState,
    val toastsState: ToastsState
): ViewModel() {

}