package com.artem.tusaandroid.app

import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import com.artem.tusaandroid.app.action.friends.FriendsState
import com.artem.tusaandroid.app.login.LoginDto
import com.artem.tusaandroid.app.login.LoginResponseDto
import com.artem.tusaandroid.app.login.SendCodeDto
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.requests.CustomTucikEndpoints
import com.artem.tusaandroid.socket.SocketListener
import com.squareup.moshi.Moshi
import com.squareup.moshi.adapter
import okhttp3.MediaType.Companion.toMediaTypeOrNull
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody.Companion.toRequestBody

class AuthenticationState(
    private val okHttpClient: OkHttpClient,
    private val customTucikEndpoints: CustomTucikEndpoints,
    private val profileState: ProfileState,
    private val meAvatarState: MeAvatarState,
    private val friendsState: FriendsState,
    private val moshi: Moshi,
    private val socketListener: SocketListener
) {
    var authenticated by mutableStateOf(profileState.getIsAuthenticated())

    fun logout() {
        profileState.saveJwt("")
        authenticated = false
        socketListener.disconnect()
        profileState.clear()
        meAvatarState.hideMe()
        meAvatarState.clearAvatar()

        val logoutUrl = customTucikEndpoints.makeLogout()
        val request = Request.Builder()
            .url(logoutUrl)
            .post("".toRequestBody())
            .build()

        try {
            okHttpClient.newCall(request).execute().use { response ->
                if (response.isSuccessful.not())
                    return
            }
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    @OptIn(ExperimentalStdlibApi::class)
    fun sendCodeToPhone(phone: String): Boolean {
        val request = Request.Builder()
            .url(customTucikEndpoints.makeSendCode())
            .post(
                moshi.adapter<SendCodeDto>().toJson(SendCodeDto(phone))
                    .toRequestBody("application/json".toMediaTypeOrNull())
            )
            .build()
        val response = okHttpClient.newCall(request).execute()
        return response.isSuccessful
    }

    @OptIn(ExperimentalStdlibApi::class)
    fun login(phone: String, code: String): Boolean {
        val request = Request.Builder()
            .url(customTucikEndpoints.makeLoginEndpoint())
            .post(
                moshi.adapter<LoginDto>().toJson(LoginDto(phone, code))
                    .toRequestBody("application/json".toMediaTypeOrNull())
            )
            .build()
        val response = okHttpClient.newCall(request).execute()
        if (response.isSuccessful) {
            val loginResponse = moshi.adapter<LoginResponseDto>().fromJson(response.body?.string()!!)!!
            profileState.saveJwt(loginResponse.jwt)
            profileState.saveUserId(loginResponse.id)
            profileState.saveName(loginResponse.name)
            profileState.saveUniqueName(loginResponse.uniqueName)
            profileState.savePhone(loginResponse.phone)
            socketListener.connect(loginResponse.id)
            authenticated = true
        }
        return response.isSuccessful
    }
}