package com.artem.tusaandroid.app.map

import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.AppVariables
import com.artem.tusaandroid.app.MeAvatarState
import com.artem.tusaandroid.location.LastLocationState
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
open class MapViewModel @Inject constructor(
    val appVariables: AppVariables?,
    val lastLocationState: LastLocationState?,
    val meAvatarState: MeAvatarState?
): ViewModel()

