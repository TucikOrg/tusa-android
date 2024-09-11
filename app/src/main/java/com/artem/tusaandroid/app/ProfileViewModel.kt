package com.artem.tusaandroid.app

import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.AppVariables
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
class ProfileViewModel @Inject constructor(
    private val appVariables: AppVariables
): ViewModel() {
    var profileWindow by mutableStateOf(ProfileWindow.CLOSE)
    lateinit var onResult: (ProfileResult) -> Unit

    fun dismiss() {
        profileWindow = ProfileWindow.CLOSE
    }

    fun logout() {
        onResult.invoke(ProfileResult.logout())
        appVariables.saveJwt("")
        dismiss()
    }
}