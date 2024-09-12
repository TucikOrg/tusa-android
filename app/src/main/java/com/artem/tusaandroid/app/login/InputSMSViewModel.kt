package com.artem.tusaandroid.app.login

import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.setValue
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.api.AuthenticationControllerApi
import com.artem.tusaandroid.app.AuthenticationState
import com.artem.tusaandroid.app.action.MainActionFabViewModel
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.model.LoginDto
import com.artem.tusaandroid.model.SendCodeDto
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import org.openapitools.client.infrastructure.ClientException
import javax.inject.Inject

@HiltViewModel
open class InputSMSViewModel @Inject constructor(
    private val profileState: ProfileState?,
    private val authenticationControllerApi: AuthenticationControllerApi?,
    private val authenticationState: AuthenticationState?
): ViewModel() {
    private val initTimeLeft = profileState?.timeLeftInit?: 40
    var timeLeft by mutableIntStateOf(initTimeLeft)

    fun getPhone(): String {
        return profileState?.getPhone()?: ""
    }

    fun makeTimeLeftText(): String {
        return if (timeLeft > 0) "Можно отправить повторно через $timeLeft секунд" else "Отправить повторно"
    }

    fun sendCodeToPhone(phone: String) {
        profileState?.savePhone(phone)
        viewModelScope.launch {
            withContext(Dispatchers.IO) {
                try {
                    authenticationControllerApi?.sendCode(SendCodeDto(phone))
                } catch (clientException: ClientException) {
                    clientException.printStackTrace()
                }
            }
        }
    }

    fun login(code: String, phone: String, mainActionFabViewModel: MainActionFabViewModel) {
        viewModelScope.launch {
            withContext(Dispatchers.IO) {
                try {
                    val response = authenticationControllerApi?.login(LoginDto(code = code, phone = phone))?: return@withContext
                    profileState?.saveJwt(response.jwt)
                    profileState?.saveName(response.name)
                    authenticationState?.authenticated = true
                    if (profileState?.getName().isNullOrBlank()) {
                        mainActionFabViewModel.stage = MainActionStage.INPUT_NAME
                    } else {
                        mainActionFabViewModel.stage = MainActionStage.PROFILE
                        mainActionFabViewModel.showModal = false
                    }
                } catch (clientException: ClientException) {
                    clientException.printStackTrace()
                }
            }
        }
    }

}