package com.artem.tusaandroid.app

import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.app.systemui.SystemUIState
import com.artem.tusaandroid.cropper.CropperState
import com.artem.tusaandroid.location.LastLocationState
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
class MainActivityViewModel @Inject constructor(
    val authenticationState: AuthenticationState,
    val profileState: ProfileState,
    val systemUIState: SystemUIState,
    val lastLocationState: LastLocationState
): ViewModel()