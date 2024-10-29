package com.artem.tusaandroid.app.action

import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.app.AuthenticationState
import com.artem.tusaandroid.app.MeAvatarState
import com.artem.tusaandroid.app.login.MainActionStage
import com.artem.tusaandroid.app.profile.ProfileState
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import javax.inject.Inject

@HiltViewModel
open class MainActionFabViewModel @Inject constructor(
    val authenticationState: AuthenticationState?
): ViewModel() {
    var showModal by mutableStateOf(false)
    var stage by mutableStateOf(getInitStage())

    private fun getInitStage(): MainActionStage {
        if (authenticationState?.authenticated == true) {
            return MainActionStage.PROFILE
        }
        return MainActionStage.INPUT_PHONE
    }

    fun logout() {
        viewModelScope.launch {
            withContext(Dispatchers.IO) {
                showModal = false
                stage = MainActionStage.INPUT_PHONE
                authenticationState?.logout()
            }
        }
    }
}