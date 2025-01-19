package com.artem.tusaandroid.app.logs

import android.content.Context
import androidx.work.Data
import androidx.work.Worker
import androidx.work.WorkerParameters
import okhttp3.MediaType.Companion.toMediaTypeOrNull
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody.Companion.toRequestBody

class CrashLogUploadWorker(context: Context, params: WorkerParameters) : Worker(context, params) {
    override fun doWork(): Result {
        val json = inputData.getString("json") ?: return Result.failure()
        val url = inputData.getString("url") ?: return Result.failure()

        val client = OkHttpClient()
        val request = Request.Builder()
            .url(url)
            .post(json.toRequestBody("application/json".toMediaTypeOrNull()))
            .build()
        val response = client.newCall(request).execute()
        if (response.isSuccessful.not()) {
            return Result.failure(Data.Builder().putString("error", response.body?.string()).build())
        }

        return Result.success()
    }
}