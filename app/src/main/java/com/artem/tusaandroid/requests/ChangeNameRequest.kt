package com.artem.tusaandroid.requests

import com.artem.tusaandroid.AppVariables
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody.Companion.toRequestBody

fun changeNameRequest(coroutineScope: CoroutineScope, newName: String) {
    coroutineScope.launch {
        withContext(Dispatchers.IO) {
            val endpoint = AppVariables.getInstance().makeEndpoint("api/v1/profile/name")
            val okHttpClient = OkHttpClient()
            val request = Request.Builder()
                .url(endpoint)
                .post(newName.toRequestBody(null))
                .header("Authorization", "Bearer ${AppVariables.getInstance().getJwt()}")
                .build()
            okHttpClient.newCall(request).execute().use { response ->
                if (response.isSuccessful) {
                    AppVariables.getInstance().saveName(newName)
                }
            }
        }
    }
}

