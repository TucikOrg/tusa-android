package com.artem.tusaandroid.app.admin

import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.dto.User
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject


@HiltViewModel
open class ForAdminUserRowViewModel @Inject constructor(
    private var profileState: ProfileState?
): ViewModel() {
    fun checkProfileState(user: User) {
        profileState?.checkProfileState(user)
    }

    fun getName(userId: Long): MutableState<String> {
        if (profileState == null) {
            return mutableStateOf("Preview")
        }
        return profileState!!.getProfile(userId).name
    }

    fun getUniqueName(userId: Long): MutableState<String> {
        if (profileState == null) {
            return mutableStateOf("Preview")
        }
        return profileState!!.getProfile(userId).uniqueName
    }
}