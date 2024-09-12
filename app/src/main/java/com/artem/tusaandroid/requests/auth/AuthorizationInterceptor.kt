package com.artem.tusaandroid.requests.auth

import com.artem.tusaandroid.app.profile.ProfileState
import okhttp3.Interceptor
import okhttp3.Request
import okhttp3.Response

class AuthorizationInterceptor(
    private val profileState: ProfileState
) : Interceptor {
    override fun intercept(chain: Interceptor.Chain): Response {
        val newRequest = chain.request().signedRequest()
        return chain.proceed(newRequest)
    }

    private fun Request.signedRequest(): Request {
        if (profileState.getIsAuthenticated()) {
            return this.newBuilder()
                .header("Authorization", "Bearer ${profileState.getJwt()}")
                .build()
        }

        return this.newBuilder().build()
    }
}