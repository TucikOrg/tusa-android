package com.artem.tusaandroid.app.name

import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.AppVariables
import com.artem.tusaandroid.api.ProfileControllerApi
import com.artem.tusaandroid.model.ChangeNameDto
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import javax.inject.Inject

@HiltViewModel
open class InputNameInModalViewModel @Inject constructor(
    private val appVariables: AppVariables?,
    private val profileControllerApi: ProfileControllerApi?
): ViewModel() {
    var name by mutableStateOf(appVariables?.getName())

    fun changeName(name: String) {
        this.name = name
        appVariables?.saveName(name)
        viewModelScope.launch {
            withContext(Dispatchers.IO) {
                profileControllerApi?.changeName(ChangeNameDto(name))
            }
        }
    }
}