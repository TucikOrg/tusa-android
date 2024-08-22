package com.artem.tusaandroid.requests

import com.artem.tusaandroid.AppVariables
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody.Companion.toRequestBody

data class ChangeUniqueNameResult(
    var saved: Boolean,
    var isOk: Boolean
)

fun changeUniqueNameRequest(coroutineScope: CoroutineScope, newUniqueName: String, onDone: (ChangeUniqueNameResult) -> Unit) {
    coroutineScope.launch {
        withContext(Dispatchers.IO) {
            val endpoint = AppVariables.getInstance().makeEndpoint("api/v1/profile/userUniqueName")
            val okHttpClient = OkHttpClient()
            val request = Request.Builder()
                .url(endpoint)
                .post(newUniqueName.toRequestBody(null))
                .header("Authorization", "Bearer ${AppVariables.getInstance().getJwt()}")
                .build()
            okHttpClient.newCall(request).execute().use { response ->
                var saved = false
                if (response.isSuccessful) {
                    val body = response.body?.string() ?: ""
                    saved = body == "true"
                    if (saved) {
                        AppVariables.getInstance().saveUniqueName(newUniqueName)
                    }
                }
                onDone.invoke(ChangeUniqueNameResult(saved, response.isSuccessful))
            }
        }
    }
}