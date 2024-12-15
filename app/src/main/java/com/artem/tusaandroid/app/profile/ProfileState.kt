package com.artem.tusaandroid.app.profile

import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import com.artem.tusaandroid.StateHasSharedPreferences
import com.artem.tusaandroid.dto.Profile
import com.artem.tusaandroid.dto.User
import com.artem.tusaandroid.socket.SocketBinaryMessage
import com.artem.tusaandroid.socket.SocketListener
import kotlinx.serialization.ExperimentalSerializationApi
import kotlinx.serialization.cbor.Cbor
import kotlinx.serialization.encodeToByteArray

open class ProfileState(
    private val socketListener: SocketListener?
): StateHasSharedPreferences() {
    val timeLeftInit = 40

    private var name: MutableState<String> = mutableStateOf("")
    private var uniqueName: MutableState<String> = mutableStateOf("")
    private var phone: MutableState<String> = mutableStateOf("")
    private var jwt: String? = null
    private var userId: Long? = null

    private var profiles: MutableMap<Long, Profile> = mutableMapOf()

    fun getProfile(userId: Long): Profile {
        if (profiles[userId] == null) {
            profiles[userId] = Profile(
                name = mutableStateOf(""),
                uniqueName = mutableStateOf(""),
                gmail = mutableStateOf("")
            )
        }
        return profiles[userId]!!
    }

    fun changeName(name: String, userId: Long) {
        socketListener?.getSendMessage()?.changeName(name, userId)
        getProfile(userId).name.value = name
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun changeName(name: String) {
        socketListener?.getSendMessage()?.sendMessage(SocketBinaryMessage("change-name", Cbor.encodeToByteArray(name)))
        this.name.value = name
        saveName(name)
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun changeUniqueName(uniqueName: String, userId: Long) {
        socketListener?.getSendMessage()?.changeUniqueName(uniqueName, userId)
        getProfile(userId).uniqueName.value = uniqueName
    }

    @OptIn(ExperimentalSerializationApi::class)
    fun changeUniqueName(uniqueName: String) {
        socketListener?.getSendMessage()?.sendMessage(SocketBinaryMessage("change-unique-name", Cbor.encodeToByteArray(uniqueName)))
        this.uniqueName.value = uniqueName
        saveUniqueName(uniqueName)
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
        return getJwt().isNotEmpty()
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

    fun getName(userId: Long): MutableState<String> {
        return getProfile(userId).name
    }

    fun getUniqueName(): MutableState<String> {
        return uniqueName
    }

    fun getUniqueName(userId: Long): MutableState<String> {
        return getProfile(userId).uniqueName
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

    fun checkProfileState(user: User) {
        val profile = getProfile(user.id)
        if (user.uniqueName != null) profile.uniqueName.value = user.uniqueName
        if (user.name != null) profile.name.value = user.name
        if (user.gmail != null) profile.gmail.value = user.gmail
    }
}