package com.artem.tusaandroid.app

import android.graphics.BitmapFactory
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.AppVariables
import com.artem.tusaandroid.api.AvatarControllerApi
import com.artem.tusaandroid.requests.CustomTucikEndpoints
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import okhttp3.OkHttpClient
import okhttp3.Request
import org.openapitools.client.infrastructure.ClientException
import javax.inject.Inject

@HiltViewModel
class LoadMeAvatarViewModel @Inject constructor(
    private val meAvatarState: MeAvatarState,
    private val avatarControllerApi: AvatarControllerApi,
    private val appVariables: AppVariables,
    private val okHttpClient: OkHttpClient,
    private val customTucikEndpoints: CustomTucikEndpoints
): ViewModel() {
    fun loadMeAvatar() {
        viewModelScope.launch {
            withContext(Dispatchers.IO) {
                try {
                    val phone = appVariables.getPhone()
                    val request = Request.Builder()
                        .url(customTucikEndpoints.makeToAvatarImage(phone))
                        .get()
                        .build()
                    okHttpClient.newCall(request).execute().use { response ->
                        if (response.isSuccessful.not()) {
                            return@use
                        }

                        val bytes = response.body!!.bytes()
                        val bitmap = BitmapFactory.decodeByteArray(bytes, 0, bytes.size)
                        meAvatarState.setAvatar(bitmap, bytes)
                    }
                } catch (clientException: ClientException) {
                    clientException.printStackTrace()
                }
            }
        }
    }
}