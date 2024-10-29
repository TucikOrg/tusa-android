package com.artem.tusaandroid.app.profile

import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import com.artem.tusaandroid.StateHasSharedPreferences
import com.artem.tusaandroid.socket.SendMessage
import com.artem.tusaandroid.socket.SocketBinaryMessage
import kotlinx.serialization.ExperimentalSerializationApi
import kotlinx.serialization.cbor.Cbor
import kotlinx.serialization.encodeToByteArray

class ProfileState(
    private val sendMessage: SendMessage?
): StateHasSharedPreferences() {
    val timeLeftInit = 40

    private var name: MutableState<String> = mutableStateOf("")
    private var uniqueName: MutableState<String> = mutableStateOf("")
    private var phone: MutableState<String> = mutableStateOf("")
    private var jwt: String? = null
    private var userId: Long? = null

    @OptIn(ExperimentalSerializationApi::class)
    fun changeName(name: String) {
        sendMessage?.sendMessage(SocketBinaryMessage("change-name", Cbor.encodeToByteArray(name)))
        this.name.value = name
        saveName(name)
    }

    fun clear() {
        jwt = null
        userId = null
        phone.value = ""
        name.value = ""
        uniqueName.value = ""
    }

    fun saveJwt(jwt: String) {
        this.jwt = jwt
        JwtGlobal.jwt = jwt
        sharedPreferences.edit().putString("jwt", jwt).apply()
    }

    fun saveUserId(userId: Long) {
        this.userId = userId
        sharedPreferences.edit().putLong("userId", userId).apply()
    }

    fun savePhone(phone: String) {
        this.phone.value = phone
        sharedPreferences.edit().putString(getKey() + "phone", phone).apply()
    }

    fun saveName(name: String) {
        this.name.value = name
        sharedPreferences.edit().putString(getKey() + "name", name).apply()
    }

    fun saveUniqueName(uniqueName: String) {
        this.uniqueName.value = uniqueName
        sharedPreferences.edit().putString(getKey() + "uniqueName", uniqueName).apply()
    }

    fun getIsAuthenticated(): Boolean {
        return jwt != null
    }

    fun getJwt(): String {
        return jwt?: ""
    }

    fun getUserId(): Long {
        return userId?: -1
    }

    fun getKey(): String {
        return userId.toString()
    }

    fun getPhone(): MutableState<String> {
        return phone
    }

    fun getName(): MutableState<String> {
        return name
    }

    fun getUniqueName(): MutableState<String> {
        return uniqueName
    }

    override fun onLoad(userId: Long) {
        jwt = sharedPreferences.getString("jwt", null)
        JwtGlobal.jwt = jwt
        this.userId = sharedPreferences.getLong("userId", userId)
        val key = this.userId.toString()

        uniqueName.value = sharedPreferences.getString(key + "uniqueName", "")!!
        phone.value = sharedPreferences.getString(key + "phone", "")!!
        name.value = sharedPreferences.getString(key + "name", "")!!
    }
}