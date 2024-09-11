package com.artem.tusaandroid.requests.auth

import com.artem.tusaandroid.AppVariables
import okhttp3.Interceptor
import okhttp3.Request
import okhttp3.Response

class AuthorizationInterceptor(
    private val appVariables: AppVariables
) : Interceptor {
    override fun intercept(chain: Interceptor.Chain): Response {
        val newRequest = chain.request().signedRequest()
        return chain.proceed(newRequest)
    }

    private fun Request.signedRequest(): Request {
        if (appVariables.getIsAuthenticated()) {
            return this.newBuilder()
                .header("Authorization", "Bearer ${appVariables.getJwt()}")
                .build()
        }

        return this.newBuilder().build()
    }
}