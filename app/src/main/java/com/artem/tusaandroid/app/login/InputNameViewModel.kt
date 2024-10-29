package com.artem.tusaandroid.app.login

import androidx.compose.runtime.mutableStateOf
import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.app.action.MainActionFabViewModel
import com.artem.tusaandroid.app.profile.ProfileState
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
class InputNameViewModel @Inject constructor(
    private val profileState: ProfileState?
): ViewModel() {
    fun getName() = profileState?.getName()?: mutableStateOf("Name")

    fun changeName(name: String, rootModel: MainActionFabViewModel) {
        profileState?.changeName(name)
        rootModel.stage = MainActionStage.PROFILE
        rootModel.showModal = false
    }
}