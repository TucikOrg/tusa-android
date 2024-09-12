package com.artem.tusaandroid.app

import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.requests.CustomTucikEndpoints
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody.Companion.toRequestBody

class AuthenticationState(
    private val profileState: ProfileState,
    private val okHttpClient: OkHttpClient,
    private val customTucikEndpoints: CustomTucikEndpoints
) {
    var authenticated by mutableStateOf(profileState.getIsAuthenticated())

    fun logout() {
        profileState.saveJwt("")
        authenticated = false

        val request = Request.Builder()
            .url(customTucikEndpoints.makeLogout())
            .post("".toRequestBody())
            .build()

        okHttpClient.newCall(request).execute().use { response ->
            if (response.isSuccessful.not())
                return
        }
    }
}