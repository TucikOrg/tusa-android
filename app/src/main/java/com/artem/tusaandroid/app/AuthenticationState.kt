package com.artem.tusaandroid.app

import android.content.Context
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.credentials.ClearCredentialStateRequest
import androidx.credentials.CredentialManager
import androidx.credentials.GetCredentialRequest
import androidx.credentials.GetCredentialResponse
import com.artem.tusaandroid.app.login.LoginResponseDto
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.requests.CustomTucikEndpoints
import com.artem.tusaandroid.socket.SocketListener
import com.google.android.libraries.identity.googleid.GetGoogleIdOption
import com.google.android.libraries.identity.googleid.GoogleIdTokenCredential
import com.squareup.moshi.Moshi
import com.squareup.moshi.adapter
import okhttp3.MediaType.Companion.toMediaTypeOrNull
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody.Companion.toRequestBody
import java.security.SecureRandom
import java.util.Base64
import com.artem.tusaandroid.R

class AuthenticationState(
    private val okHttpClient: OkHttpClient,
    private val customTucikEndpoints: CustomTucikEndpoints,
    private val profileState: ProfileState,
    private val meAvatarState: MeAvatarState,
    private val moshi: Moshi,
    private val socketListener: SocketListener
) {
    var authenticated by mutableStateOf(profileState.getIsAuthenticated())

    suspend fun login(activityContext: Context) {

        val credentialManager = CredentialManager.create(activityContext)

        val googleIdOption: GetGoogleIdOption = GetGoogleIdOption.Builder()
            .setFilterByAuthorizedAccounts(false)
            .setServerClientId(activityContext.getString(R.string.web_client_id))
            .setNonce(generateChallenge())
            .build()

        val request: GetCredentialRequest = GetCredentialRequest.Builder()
            .addCredentialOption(googleIdOption)
            .build()

        try {
            val result = credentialManager.getCredential(
                request = request,
                context = activityContext,
            )
            handleSignIn(result)
        } catch (exception: Exception) {
            // всплыввет если например пользователь закрыл окошко с гугл авторизацией
            exception.printStackTrace()
        }
    }

    @OptIn(ExperimentalStdlibApi::class)
    suspend fun handleSignIn(result: GetCredentialResponse) {
        val credential = result.credential
        if (credential.type != GoogleIdTokenCredential.TYPE_GOOGLE_ID_TOKEN_CREDENTIAL) {
            return
        }

        try {
            // Use googleIdTokenCredential and extract the ID to validate and
            // authenticate on your server.
            val googleIdTokenCredential = GoogleIdTokenCredential.createFrom(credential.data)
            val idToken = googleIdTokenCredential.idToken

            // You can use the members of googleIdTokenCredential directly for UX
            // purposes, but don't use them to store or control access to user
            // data. For that you first need to validate the token:
            // pass googleIdTokenCredential.getIdToken() to the backend server.
            val request = Request.Builder()
                .url(customTucikEndpoints.makeGoogleSignIn())
                .post(idToken.toRequestBody("application/json".toMediaTypeOrNull()))
                .build()
            val response = okHttpClient.newCall(request).execute()
            if (response.isSuccessful) {
                // The server validated the token and created a session.
                // Proceed to the next screen.
                val loginResponse = moshi.adapter<LoginResponseDto>().fromJson(response.body?.string()!!)!!
                profileState.saveJwt(loginResponse.jwt)
                profileState.saveUserId(loginResponse.id)
                profileState.saveName(loginResponse.name)
                profileState.saveUniqueName(loginResponse.uniqueName)
                profileState.savePhone(loginResponse.phone)
                socketListener.connect(loginResponse.id)
                authenticated = true
            } else {
                // The server couldn't validate the token.
                // Show an error message to the user.
            }
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    suspend fun logout(activityContext: Context) {
        val credentialManager = CredentialManager.create(activityContext)
        credentialManager.clearCredentialState(ClearCredentialStateRequest())

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

    private fun generateChallenge(): String {
        val challengeBytes = ByteArray(32) // 32 bytes (256 bits) is a common length for challenges
        SecureRandom().nextBytes(challengeBytes)
        return Base64.getUrlEncoder().withoutPadding().encodeToString(challengeBytes)
    }
}