package com.artem.tusaandroid.notification

import android.content.Context
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import androidx.lifecycle.ViewModel
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
open class NotificationsEnabledCheckViewModel @Inject constructor(
    private val notificationsService: NotificationsService?
): ViewModel() {
    fun getShowDialog(): MutableState<Boolean> = notificationsService?.showDialog ?: mutableStateOf(false)

    fun goToSettings(context: Context) {
        notificationsService?.goToSettings(context)
    }

    fun check(context: Context) {
        notificationsService?.check(context)
    }
}