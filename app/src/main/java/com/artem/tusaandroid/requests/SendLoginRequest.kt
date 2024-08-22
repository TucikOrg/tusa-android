package com.artem.tusaandroid.requests

import com.artem.tusaandroid.AppVariables
import com.artem.tusaandroid.dto.LoginDto
import com.artem.tusaandroid.dto.LoginResponseDto
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import kotlinx.serialization.json.Json
import okhttp3.MediaType.Companion.toMediaType
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody.Companion.toRequestBody

data class LoginRequestResult(
    val loginResponse: LoginResponseDto?,
    val isCodeInvalid: Boolean,
    val isOk: Boolean
)

fun sendLoginRequest(coroutineScope: CoroutineScope, code: String, onResponse: (LoginRequestResult) -> Unit) {
    coroutineScope.launch {
        withContext(Dispatchers.IO) {
            val endpoint = AppVariables.getInstance().makeEndpoint("api/v1/auth/login")
            val okHttpClient = OkHttpClient()
            val dto = LoginDto(
                AppVariables.getInstance().getPhone(),
                code,
                AppVariables.getInstance().getDeviceAppIdentity()
            )

            val json = Json.encodeToString(LoginDto.serializer(), dto)
            val request = Request.Builder().url(endpoint).post(json.toRequestBody("application/json".toMediaType())).build()
            okHttpClient.newCall(request).execute().use { response ->
                val body = response.body?.string()?: ""
                val isCodeInvalid = body == "Invalid code"
                val isOk = response.isSuccessful
                var loginResponse: LoginResponseDto? = null
                if (isOk) {
                    loginResponse = Json.decodeFromString(LoginResponseDto.serializer(), body)
                }
                onResponse.invoke(LoginRequestResult(loginResponse, isCodeInvalid, isOk))
            }
        }
    }
}