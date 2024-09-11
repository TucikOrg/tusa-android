package com.artem.tusaandroid

import android.content.SharedPreferences

abstract class StateHasSharedPreferences {
    protected lateinit var sharedPreferences: SharedPreferences

    fun load(sharedPreferences: SharedPreferences) {
        this.sharedPreferences = sharedPreferences
        onLoad()
    }

    abstract fun onLoad()
}