package com.artem.tusaandroid.app.action.auth

import android.content.Context
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.app.AuthenticationState
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import javax.inject.Inject

@HiltViewModel
open class CredentialsManagerAuthViewModel @Inject constructor(
    val authenticationState: AuthenticationState
): ViewModel() {
    fun enter(activityContext: Context) {
        viewModelScope.launch {
            withContext(Dispatchers.IO) {
                authenticationState.login(activityContext)
            }
        }
    }
}