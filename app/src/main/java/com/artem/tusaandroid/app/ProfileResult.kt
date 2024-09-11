package com.artem.tusaandroid.app

data class ProfileResult(
    val logout: Boolean
) {
    companion object {
        fun logout() = ProfileResult(true)
    }
}