package com.artem.tusaandroid.app

import androidx.lifecycle.ViewModel
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
class MainActivityViewModel @Inject constructor(
    val authenticationState: AuthenticationState
): ViewModel() {

}