package com.artem.tusaandroid.app.avatar

import android.content.Context
import android.graphics.Bitmap
import androidx.lifecycle.ViewModel
import androidx.work.Data
import androidx.work.OneTimeWorkRequestBuilder
import androidx.work.WorkManager
import com.artem.tusaandroid.app.MeAvatarState
import com.artem.tusaandroid.app.profile.JwtGlobal
import com.artem.tusaandroid.requests.CustomTucikEndpoints
import dagger.hilt.android.lifecycle.HiltViewModel
import java.io.ByteArrayOutputStream
import java.io.File
import java.io.FileOutputStream
import javax.inject.Inject

@HiltViewModel
open class MeAvatarViewModel @Inject constructor(
    private val meAvatarState: MeAvatarState,
    private val customTucikEndpoints: CustomTucikEndpoints?
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

            val uploadWorkRequest = OneTimeWorkRequestBuilder<AvatarUploadWorker>()
                .setInputData(
                    Data.Builder()
                        .putString("file_path", tempFile.path)
                        .putString("jwt", JwtGlobal.jwt)
                        .putString("url", customTucikEndpoints?.makeAvatarUpload())
                        .build()
                )
                .build()
            WorkManager.getInstance(context).enqueue(uploadWorkRequest)
        }
    }
}