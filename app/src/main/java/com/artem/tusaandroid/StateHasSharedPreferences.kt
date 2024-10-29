package com.artem.tusaandroid

import android.content.SharedPreferences

abstract class StateHasSharedPreferences {
    protected lateinit var sharedPreferences: SharedPreferences

    fun load(sharedPreferences: SharedPreferences, userId: Long) {
        this.sharedPreferences = sharedPreferences
        onLoad(userId)
    }

    abstract fun onLoad(userId: Long)
}