package com.artem.tusaandroid.app.action

import android.content.Context
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.app.AuthenticationState
import com.artem.tusaandroid.app.profile.ProfileState
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import javax.inject.Inject

@HiltViewModel
open class MainActionFabViewModel @Inject constructor(
    val authenticationState: AuthenticationState?,
    val profileState: ProfileState?
): ViewModel() {
    var showModal by mutableStateOf(false)

    fun logout(context: Context) {
        viewModelScope.launch {
            withContext(Dispatchers.IO) {
                showModal = false
                authenticationState?.logout(context)
            }
        }
    }
}