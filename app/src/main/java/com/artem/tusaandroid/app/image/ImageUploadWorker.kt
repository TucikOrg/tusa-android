package com.artem.tusaandroid.app.image

import android.content.Context
import androidx.work.Data
import androidx.work.Worker
import androidx.work.WorkerParameters
import okhttp3.MediaType.Companion.toMediaTypeOrNull
import okhttp3.MultipartBody
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody.Companion.asRequestBody
import java.io.File

class ImageUploadWorker(context: Context, params: WorkerParameters) : Worker(context, params) {
    override fun doWork(): Result {
        val filePath = inputData.getString("file_path") ?: return Result.failure()
        val jwt = inputData.getString("jwt") ?: return Result.failure()
        val url = inputData.getString("url") ?: return Result.failure()
        val fileId = inputData.getString("file_id") ?: return Result.failure()
        val file = File(filePath)
        if (!file.exists()) return Result.failure()

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
        val response = client.newCall(request).execute()
        if (response.isSuccessful.not()) {
            return Result.failure(Data.Builder().putString("error", response.body?.string()).build())
        }

        val idStr = response.body!!.string().replace(" ", "")
        val id = idStr.toLong()
        val resultData = Data.Builder().putLong("id", id).build()
        return Result.success(resultData)
    }
}