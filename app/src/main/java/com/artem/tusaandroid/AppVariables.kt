package com.artem.tusaandroid

import android.content.SharedPreferences
import android.net.Uri
import java.util.UUID

class AppVariables {

    fun getDeviceAppIdentity(): String {
        return deviceAppIdentity!!
    }

    val url = "https://4573-193-176-214-47.ngrok-free.app"
    var avatarSelectedUri: Uri? = null
    var avatarId: String? = null
    private var name: String? = null
    private var uniqueName: String? = null
    private var phone: String? = null
    private var jwt: String? = null
    private lateinit var sharedPreferences: SharedPreferences
    private var deviceAppIdentity: String? = null
    private val hash: MutableMap<String, Any> = mutableMapOf()

    fun setSharedPreference(sharedPreferences: SharedPreferences) {
        this.sharedPreferences = sharedPreferences
    }

    fun nameOrEmpty(): String {
        return name?: ""
    }

    fun makeEndpoint(path: String): String {
        return "$url/$path"
    }

    fun saveToMem(key: String, value: Any) {
        hash[key] = value
    }

    fun <T> getFromMem(key: String): T  {
        return hash[key] as T
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

    fun saveUniqueName(uniqueName: String) {
        this.uniqueName = uniqueName
        sharedPreferences.edit().putString("uniqueName", uniqueName).apply()
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

    fun getUniqueName(): String {
        return uniqueName?: ""
    }

    fun load() {
        deviceAppIdentity = sharedPreferences.getString("deviceAppIdentity", null)
        if (deviceAppIdentity == null) {
            deviceAppIdentity = UUID.randomUUID().toString()
            sharedPreferences.edit().putString("deviceAppIdentity", deviceAppIdentity).apply()
        }
        avatarId = sharedPreferences.getString("avatarId", null)
        uniqueName = sharedPreferences.getString("uniqueName", null)
        phone = sharedPreferences.getString("phone", null)
        name = sharedPreferences.getString("name", null)
        jwt = sharedPreferences.getString("jwt", null)
    }

    companion object {
        private var instance: AppVariables? = null

        fun getInstance(): AppVariables {
            if (instance == null) {
                instance = AppVariables()
            }
            return instance!!
        }
    }
}