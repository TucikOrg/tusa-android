package com.artem.tusaandroid.location

import androidx.compose.runtime.mutableStateOf
import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.AppVariables
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
open class LocationSetupCardViewModel @Inject constructor(
    private val lastLocationState: LastLocationState?
): ViewModel() {
    var locationServiceStarted = mutableStateOf(lastLocationState?.getLocationForegroundServiceStarted())
}