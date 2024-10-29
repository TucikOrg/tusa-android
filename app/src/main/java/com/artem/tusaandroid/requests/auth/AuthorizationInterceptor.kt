package com.artem.tusaandroid.requests.auth

import com.artem.tusaandroid.app.profile.JwtGlobal
import okhttp3.Interceptor
import okhttp3.Request
import okhttp3.Response

class AuthorizationInterceptor : Interceptor {
    override fun intercept(chain: Interceptor.Chain): Response {
        val newRequest = chain.request().signedRequest()
        return chain.proceed(newRequest)
    }

    private fun Request.signedRequest(): Request {
        if (JwtGlobal.jwt?.isNotEmpty() == true) {
            return this.newBuilder()
                .header("Authorization", "Bearer ${JwtGlobal.jwt!!}")
                .build()
        }

        return this.newBuilder().build()
    }
}