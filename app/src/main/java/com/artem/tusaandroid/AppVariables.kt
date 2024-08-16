package com.artem.tusaandroid

import android.content.SharedPreferences
import android.net.Uri
import java.util.UUID

class AppVariables {

    fun getDeviceAppIdentity(): String {
        return deviceAppIdentity!!
    }

    val url = "http://192.168.0.103:8080"
    var avatarSelectedUri: Uri? = null
    var avatarId: String? = null
    var name: String? = null
    var username: String? = null
    var phone: String? = null
    private var deviceAppIdentity: String? = null
    private val hash: MutableMap<String, Any> = mutableMapOf()

    fun saveToMem(key: String, value: Any) {
        hash[key] = value
    }

    fun <T> getFromMem(key: String): T  {
        return hash[key] as T
    }

    fun loadFromSharedPreferences(basic: SharedPreferences) {
        deviceAppIdentity = basic.getString("deviceAppIdentity", null)
        if (deviceAppIdentity == null) {
            deviceAppIdentity = UUID.randomUUID().toString()
            basic.edit().putString("deviceAppIdentity", deviceAppIdentity).apply()
        }
        avatarId = basic.getString("avatarId", null)
        username = basic.getString("username", null)
        phone = basic.getString("phone", null)
        name = basic.getString("name", null)
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