package com.artem.tusaandroid.requests

import android.net.Uri

class CustomTucikEndpoints(
    private val basePath: String
) {
    private val legalDocuments = "api/v1/legal/documents"
    private val getAvatarImage = "api/v1/avatar/image"
    private val uploadAvatar = "api/v1/avatar"
    private val login = "api/v1/auth/login"
    private val googleSignIN = "api/v1/auth/google/login"
    private val addLocation = "api/v1/location/add"
    private val sendCode = "api/v1/auth/send-code"
    private val logout = "api/v1/logout"
    private val sendLogs = "api/v1/logs"
    private val mapTitle = "api/v1/tile/map-title/"

    private fun makeEndpoint(path: String): String {
        return "$basePath/$path"
    }

    fun makeToLegalDocuments(): String {
        return makeEndpoint(legalDocuments)
    }

    fun makeToAvatarImage(userId: Long): String {
        val uri = Uri.parse(makeEndpoint(getAvatarImage)).buildUpon()
            .appendQueryParameter("userId", userId.toString())
            .build()
        return uri.toString()
    }

    fun makeLogout(): String {
        return makeEndpoint(logout)
    }

    fun makeSendCode(): String {
        val uri = Uri.parse(makeEndpoint(sendCode)).buildUpon().build()
        return uri.toString()
    }

    fun makeLoginEndpoint(): String {
        val uri = Uri.parse(makeEndpoint(login)).buildUpon().build()
        return uri.toString()
    }

    fun makeAddLocation(): String {
        val uri = Uri.parse(makeEndpoint(addLocation)).buildUpon().build()
        return uri.toString()
    }

    fun makeAvatarUpload(): String? {
        val uri = Uri.parse(makeEndpoint(uploadAvatar)).buildUpon().build()
        return uri.toString()
    }

    fun makeAvatarUploadToOtherUser(userId: Long): String? {
        val uri = Uri.parse(makeEndpoint("$uploadAvatar/$userId")).buildUpon().build()
        return uri.toString()
    }

    fun makeGoogleSignIn(): String {
        val uri = Uri.parse(makeEndpoint(googleSignIN)).buildUpon().build()
        return uri.toString()
    }

    fun makeSendLogs(): String {
        val uri = Uri.parse(makeEndpoint(sendLogs)).buildUpon().build()
        return uri.toString()
    }

    fun makeMapTitle(lat: Float, lon: Float, zoom: Int): String {
        val uri = Uri.parse(makeEndpoint("$mapTitle$lat/$lon/$zoom")).buildUpon().build()
        return uri.toString()
    }
}