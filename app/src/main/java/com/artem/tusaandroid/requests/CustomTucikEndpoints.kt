package com.artem.tusaandroid.requests

import android.net.Uri

class CustomTucikEndpoints(
    private val basePath: String
) {
    private val legalDocuments = "api/v1/legal/documents"
    private val getAvatarImage = "api/v1/avatar/image"
    private val logout = "api/v1/logout"

    private fun makeEndpoint(path: String): String {
        return "$basePath/$path"
    }

    fun makeToLegalDocuments(): String {
        return makeEndpoint(legalDocuments)
    }

    fun makeToAvatarImage(phone: String): String {
        val uri = Uri.parse(makeEndpoint(getAvatarImage)).buildUpon()
            .appendQueryParameter("phone", phone)
            .build()
        return uri.toString()
    }

    fun makeLogout(): String {
        return makeEndpoint(logout)
    }
}