package com.artem.tusaandroid.app.image

import android.content.Context
import androidx.work.CoroutineWorker
import androidx.work.Data
import androidx.work.WorkerParameters
import okhttp3.MediaType.Companion.toMediaTypeOrNull
import okhttp3.MultipartBody
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody.Companion.asRequestBody
import java.io.File
import androidx.hilt.work.HiltWorker
import com.artem.tusaandroid.room.messenger.ImageUploadingStatusDao
import com.artem.tusaandroid.room.messenger.ImageUploadingStatusEntity
import com.artem.tusaandroid.room.messenger.UploadingImageStatus
import dagger.assisted.Assisted
import dagger.assisted.AssistedInject

@HiltWorker
class ImageUploadWorker @AssistedInject constructor(
    @Assisted private val context: Context,
    @Assisted private val params: WorkerParameters,
    private val imageUploadingStatusDao: ImageUploadingStatusDao
) : CoroutineWorker(context, params) {

    override suspend fun doWork(): Result {
        val filePath = inputData.getString("file_path") ?: return Result.failure()
        val jwt = inputData.getString("jwt") ?: return Result.failure()
        val url = inputData.getString("url") ?: return Result.failure()
        val fileId = inputData.getString("file_id") ?: return Result.failure()
        val messageTempId = inputData.getString("message_temp_id") ?: return Result.failure()
        val file = File(filePath)
        if (!file.exists()) return Result.failure()

        // помечаем что картинка загружается
        imageUploadingStatusDao.insert(
            ImageUploadingStatusEntity(
                fileId,
                UploadingImageStatus.UPLOADING.ordinal
            )
        )

        val client = OkHttpClient()
        val requestBody = MultipartBody.Builder()
            .setType(MultipartBody.FORM)
            .addFormDataPart(
                "file", file.name,
                file.asRequestBody("image/*".toMediaTypeOrNull())
            )
            .addFormDataPart("fileId", fileId)
            .build()
        val request = Request.Builder()
            .url(url)
            .addHeader("Authorization", "Bearer $jwt")
            .post(requestBody)
            .build()

        var id = -1L
        try {
            val response = client.newCall(request).execute()
            if (response.isSuccessful.not()) {
                return Result.failure(Data.Builder().putString("error", response.body?.string()).build())
            }
            val idStr = response.body!!.string().replace(" ", "")
            id = idStr.toLong()
        } catch (exception: Exception) {

            // сохранем что картинка НЕ ЗАГРУЖЕНА на сервер
            imageUploadingStatusDao.insert(
                ImageUploadingStatusEntity(
                    fileId,
                    UploadingImageStatus.ERROR.ordinal
                )
            )

            return Result.failure(Data.Builder()
                .putString("error", exception.message)
                .putBoolean("uploaded", false)
                .putString("message_temp_id", messageTempId)
                .build())
        }

        // сохранем что картинка загружена успешно
        // сохранем в сообщение в базе
        imageUploadingStatusDao.insert(
            ImageUploadingStatusEntity(
                fileId,
                UploadingImageStatus.UPLOADED.ordinal
            )
        )

        val resultData = Data.Builder()
            .putLong("id", id)
            .putString("file_id", fileId)
            .putBoolean("uploaded", true)
            .putString("message_temp_id", messageTempId)
            .build()
        return Result.success(resultData)
    }
}