package com.artem.tusaandroid.requests

import com.artem.tusaandroid.AppVariables
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody.Companion.toRequestBody
import okhttp3.Response

fun requestCodeToPhone(coroutineScope: CoroutineScope, inputPhone: String, onResponse: (Response) -> Unit) {
    coroutineScope.launch {
        withContext(Dispatchers.IO) {
            val endpoint = AppVariables.getInstance().makeEndpoint("api/v1/auth/send-code")
            val okHttpClient = OkHttpClient()
            val request = Request.Builder().url(endpoint).post(inputPhone.toRequestBody(null)).build()
            okHttpClient.newCall(request).execute().use { response ->
                onResponse.invoke(response)
            }
        }
    }
}