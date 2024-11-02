package com.artem.tusaandroid.app.login

import androidx.compose.runtime.MutableState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.app.AuthenticationState
import com.artem.tusaandroid.app.MeAvatarState
import com.artem.tusaandroid.app.action.MainActionFabViewModel
import com.artem.tusaandroid.app.action.friends.FriendsState
import com.artem.tusaandroid.app.profile.ProfileState
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import javax.inject.Inject

@HiltViewModel
open class InputSMSViewModel @Inject constructor(
    private val profileState: ProfileState?,
    private val authenticationState: AuthenticationState?,
    private val meAvatarState: MeAvatarState?,
    private val friendsState: FriendsState?
): ViewModel() {
    private val initTimeLeft = profileState?.timeLeftInit?: 40
    var timeLeft by mutableIntStateOf(initTimeLeft)

    fun getPhone(): MutableState<String> {
        return profileState?.getPhone()?: mutableStateOf("")
    }

    fun makeTimeLeftText(): String {
        return if (timeLeft > 0) "Можно отправить повторно через $timeLeft секунд" else "Отправить повторно"
    }

    fun sendCodeToPhone(phone: String) {
        viewModelScope.launch {
            withContext(Dispatchers.IO) {
                val result = authenticationState?.sendCodeToPhone(phone)
                if (result == true) {
                    timeLeft = initTimeLeft
                }
            }
        }
    }

    fun login(code: String, phone: String, mainActionFabViewModel: MainActionFabViewModel) {
        viewModelScope.launch {
            withContext(Dispatchers.IO) {
                val success = authenticationState?.login(phone, code)
                if (success == false) {
                    return@withContext
                }

                if (profileState?.getUniqueName()?.value?.isEmpty() == true) {
                    mainActionFabViewModel.stage = MainActionStage.INPUT_UNIQUE_NAME
                } else {
                    mainActionFabViewModel.stage = MainActionStage.PROFILE
                    mainActionFabViewModel.showModal = false
                }
            }
        }
    }
}