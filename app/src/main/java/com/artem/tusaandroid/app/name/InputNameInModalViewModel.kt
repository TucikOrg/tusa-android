package com.artem.tusaandroid.app.name

import androidx.compose.runtime.mutableStateOf
import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.app.profile.ProfileState
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
open class InputNameInModalViewModel @Inject constructor(
    private val profileState: ProfileState?
): ViewModel() {
    fun getName() = profileState?.getName()?: mutableStateOf("Name")

    fun changeName(name: String) {
        profileState?.changeName(name)
    }
}