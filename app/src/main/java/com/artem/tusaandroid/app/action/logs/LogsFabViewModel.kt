package com.artem.tusaandroid.app.action.logs

import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.app.logs.AppLogsState
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
open class LogsFabViewModel @Inject constructor(
    private val appLogsState: AppLogsState?,
): ViewModel() {
    fun getLogs() = appLogsState?.getLogs()
}