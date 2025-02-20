package com.artem.tusaandroid.requests

import okhttp3.Interceptor
import okhttp3.Response

class ThrottlingInterceptor(private val delayMs: Long) : Interceptor {
    override fun intercept(chain: Interceptor.Chain): Response {
        val request = chain.request()

        // Имитация медленного интернета
        Thread.sleep(delayMs)

        return chain.proceed(request)
    }
}