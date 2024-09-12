package com.artem.tusaandroid.app.profile

import com.artem.tusaandroid.StateHasSharedPreferences

class ProfileState: StateHasSharedPreferences() {
    val timeLeftInit = 40
    private var name: String? = null
    private var uniqueName: String? = null
    private var phone: String? = null
    private var jwt: String? = null

    fun saveUniqueName(uniqueName: String) {
        this.uniqueName = uniqueName
        sharedPreferences.edit().putString("uniqueName", "").apply()
    }

    fun saveJwt(jwt: String) {
        this.jwt = jwt
        sharedPreferences.edit().putString("jwt", jwt).apply()
    }

    fun savePhone(phone: String) {
        this.phone = phone
        sharedPreferences.edit().putString("phone", phone).apply()
    }

    fun saveName(name: String) {
        this.name = name
        sharedPreferences.edit().putString("name", name).apply()
    }

    fun getIsAuthenticated(): Boolean {
        return jwt != null
    }

    fun getJwt(): String {
        return jwt?: ""
    }

    fun getPhone(): String {
        return phone?: ""
    }

    fun getName(): String {
        return name?: ""
    }

    override fun onLoad() {
        uniqueName = sharedPreferences.getString("uniqueName", null)
        phone = sharedPreferences.getString("phone", null)
        name = sharedPreferences.getString("name", null)
        jwt = sharedPreferences.getString("jwt", null)
    }
}