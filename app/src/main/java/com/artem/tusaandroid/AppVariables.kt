package com.artem.tusaandroid

import android.content.SharedPreferences
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.net.Uri
import com.artem.tusaandroid.encryption.EncryptionUtils
import java.time.LocalDateTime
import java.util.Base64
import java.util.UUID
import javax.crypto.SecretKey

class AppVariables {
    private var avatarId: String? = null
    private var name: String? = null
    private var uniqueName: String? = null
    private var phone: String? = null
    private var jwt: String? = null
    private lateinit var sharedPreferences: SharedPreferences
    private var installAppIdentity: String? = null

    fun setSharedPreference(sharedPreferences: SharedPreferences) {
        this.sharedPreferences = sharedPreferences
    }

    fun getIsAuthenticated(): Boolean {
        return jwt != null
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

    fun getJwt(): String {
        return jwt?: ""
    }

    fun getPhone(): String {
        return phone?: ""
    }

    fun getName(): String {
        return name?: ""
    }

    fun load() {
        installAppIdentity = sharedPreferences.getString("installAppIdentity", null)
        if (installAppIdentity == null) {
            installAppIdentity = UUID.randomUUID().toString()
            sharedPreferences.edit().putString("installAppIdentity", installAppIdentity).apply()
        }
        avatarId = sharedPreferences.getString("avatarId", null)
        uniqueName = sharedPreferences.getString("uniqueName", null)
        phone = sharedPreferences.getString("phone", null)
        name = sharedPreferences.getString("name", null)
        jwt = sharedPreferences.getString("jwt", null)
    }

    companion object {
        const val TIME_LEFT: Int = 40
    }
}