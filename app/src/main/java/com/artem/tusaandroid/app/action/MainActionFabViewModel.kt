package com.artem.tusaandroid.app.action

import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.app.AuthenticationState
import com.artem.tusaandroid.app.login.MainActionStage
import dagger.hilt.android.lifecycle.HiltViewModel
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
        showModal = false
        stage = MainActionStage.INPUT_PHONE
        authenticationState?.logout()
    }
}