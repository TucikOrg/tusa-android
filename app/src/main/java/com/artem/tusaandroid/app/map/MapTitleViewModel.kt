package com.artem.tusaandroid.app.map

import androidx.compose.runtime.MutableState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.NativeLibrary
import com.artem.tusaandroid.requests.CustomTucikEndpoints
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import okhttp3.MediaType.Companion.toMediaTypeOrNull
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody.Companion.toRequestBody
import javax.inject.Inject

@HiltViewModel
open class MapTitleViewModel @Inject constructor(
    val updateMapTitleState: UpdateMapTitleState,
    val okHttpClient: OkHttpClient,
    val customTucikEndpoints: CustomTucikEndpoints
): ViewModel() {

    var title by mutableStateOf("")

    init {
        updateMapTitleState.onUpdateMapTitleState = {
            val cameraPos = NativeLibrary.getCameraPos()
            viewModelScope.launch(Dispatchers.IO) {
                val request = Request.Builder()
                    .url(customTucikEndpoints.makeMapTitle(
                        lat = cameraPos.latitude,
                        lon = cameraPos.longitude,
                        zoom = cameraPos.zoom.toInt())
                    )
                    .get()
                    .build()
                val response = okHttpClient.newCall(request).execute()
                if (response.isSuccessful) {
                    val body = response.body?.string()!!
                    title = body
                } else {
                    title = ""
                }
            }
        }
    }
}