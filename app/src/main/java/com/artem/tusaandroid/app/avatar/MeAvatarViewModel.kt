package com.artem.tusaandroid.app.avatar

import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.graphics.Matrix
import android.media.ExifInterface
import android.net.Uri
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import androidx.lifecycle.ViewModel
import androidx.work.Data
import androidx.work.OneTimeWorkRequestBuilder
import androidx.work.WorkManager
import com.artem.tusaandroid.app.MeAvatarState
import com.artem.tusaandroid.app.profile.JwtGlobal
import com.artem.tusaandroid.cropper.CropperState
import com.artem.tusaandroid.requests.CustomTucikEndpoints
import dagger.hilt.android.lifecycle.HiltViewModel
import java.io.ByteArrayOutputStream
import java.io.File
import java.io.FileOutputStream
import javax.inject.Inject

@HiltViewModel
open class MeAvatarViewModel @Inject constructor(
    private val meAvatarState: MeAvatarState,
    private val customTucikEndpoints: CustomTucikEndpoints?,
    private val cropperState: CropperState
): ViewModel() {
    fun getAvatarBitmap() = meAvatarState.getAvatar()

    fun avatarUriSelected(uri: Uri, context: Context) {
        var matrix: Matrix?
        var bitmap: Bitmap?

        context.contentResolver.openInputStream(uri).use { inputStream ->
            val exif = ExifInterface(inputStream!!)
            val orientation = exif.getAttributeInt(ExifInterface.TAG_ORIENTATION, ExifInterface.ORIENTATION_NORMAL)
            val rotationDegrees = when (orientation) {
                ExifInterface.ORIENTATION_ROTATE_90 -> 90
                ExifInterface.ORIENTATION_ROTATE_180 -> 180
                ExifInterface.ORIENTATION_ROTATE_270 -> 270
                else -> 0
            }
            matrix = Matrix().apply { postRotate(rotationDegrees.toFloat()) }
        }

        context.contentResolver.openInputStream(uri).use { inputStream ->
            val originalBitmap = BitmapFactory.decodeStream(inputStream)
            bitmap = Bitmap.createBitmap(originalBitmap, 0, 0, originalBitmap.width, originalBitmap.height, matrix, true)
        }
        cropperState.openCropper(bitmap!!) { cropped ->
            saveAvatarBitmap(cropped!!, context)
        }
    }

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