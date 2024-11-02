package com.artem.tusaandroid.app.login

import androidx.compose.runtime.mutableStateOf
import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.app.action.MainActionFabViewModel
import com.artem.tusaandroid.app.profile.ProfileState
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
class InputUniqueNameViewModel @Inject constructor(
    private val profileState: ProfileState?
): ViewModel() {
    fun getUniqueName() = profileState?.getUniqueName()?: mutableStateOf("Name")

    fun changeUniqueName(name: String, rootModel: MainActionFabViewModel) {
        profileState?.changeName(name)
        rootModel.stage = MainActionStage.PROFILE
        rootModel.showModal = false
    }
}