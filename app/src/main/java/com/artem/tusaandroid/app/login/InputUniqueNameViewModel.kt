package com.artem.tusaandroid.app.login

import androidx.compose.runtime.mutableStateOf
import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.app.profile.ProfileState
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
class InputUniqueNameViewModel @Inject constructor(
    private val profileState: ProfileState?
): ViewModel() {
    val showModal = mutableStateOf(profileState?.getUniqueName()?.value?.isEmpty() == true)

    fun getUniqueName() = profileState?.getUniqueName()?: mutableStateOf<String?>("")

    fun changeUniqueName(uniqueName: String?) {
        profileState?.changeUniqueName(uniqueName)
        showModal.value = false
    }
}