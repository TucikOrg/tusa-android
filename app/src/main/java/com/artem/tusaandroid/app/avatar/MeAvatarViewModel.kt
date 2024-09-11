package com.artem.tusaandroid.app.avatar

import android.content.Context
import android.graphics.Bitmap
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.api.AvatarControllerApi
import com.artem.tusaandroid.app.MeAvatarState
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import org.openapitools.client.infrastructure.ClientError
import org.openapitools.client.infrastructure.ClientException
import java.io.ByteArrayOutputStream
import java.io.File
import java.io.FileOutputStream
import javax.inject.Inject

@HiltViewModel
open class MeAvatarViewModel @Inject constructor(
    private val meAvatarState: MeAvatarState,
    private val avatarControllerApi: AvatarControllerApi?
): ViewModel() {
    fun getAvatarBitmap() = meAvatarState.getAvatar()

    fun saveAvatarBitmap(avatar: Bitmap, context: Context) {
        ByteArrayOutputStream().use {
            avatar.compress(Bitmap.CompressFormat.JPEG, 50, it)
            val bytesArray = it.toByteArray()
            meAvatarState.setAvatar(avatar, bytesArray)
            val tempFile = File.createTempFile("avatar", null, context.cacheDir)
            FileOutputStream(tempFile).use { fos ->
                fos.write(bytesArray)
            }
            viewModelScope.launch {
                withContext(Dispatchers.IO) {
                    try {
                        avatarControllerApi?.addAvatar(tempFile)
                    } catch (clientException: ClientException) {
                        clientException.printStackTrace()
                    } catch (e: Exception) {
                        e.printStackTrace()
                    }
                }
            }
        }
    }
}