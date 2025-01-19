package com.artem.tusaandroid.app.action.auth

import android.content.Context
import androidx.compose.runtime.mutableStateOf
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.app.AuthenticationState
import com.artem.tusaandroid.app.profile.ProfileState
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import javax.inject.Inject

@HiltViewModel
open class CredentialsManagerAuthViewModel @Inject constructor(
    val authenticationState: AuthenticationState?,
    val profileState: ProfileState?
): ViewModel() {
    var enabledLoginButton = mutableStateOf(true)

    fun enter(activityContext: Context) {
        viewModelScope.launch(Dispatchers.IO) {
            enabledLoginButton.value = false
            authenticationState?.login(activityContext) { result ->
                enabledLoginButton.value = true
            }
        }
    }

    fun getAgreementState(): Boolean {
        return profileState?.getAgreementState() == true
    }

    fun acceptAgreement() {
        profileState?.saveAgreementState(true)
    }
}