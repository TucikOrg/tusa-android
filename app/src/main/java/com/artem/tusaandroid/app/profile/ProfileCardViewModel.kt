package com.artem.tusaandroid.app.profile

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
open class ProfileCardViewModel @Inject constructor(
    private val profileControllerApi: ProfileControllerApi?,
    private val appVariables: AppVariables?
): ViewModel() {
    private var showModal = mutableStateOf(false)
    var name by mutableStateOf(appVariables?.getName())
    private var lastSaved: String? = name

    open fun getPhone(): String {
        return appVariables?.getPhone()?: "Phone"
    }

    fun getShowModal(): Boolean {
        return showModal.value
    }

    fun openModal() {
        showModal.value = true
    }

    fun dismiss() {
        showModal.value = false
        saveName()
    }

    fun saveName() {
        appVariables?.saveName(name!!)
        if (name == null || name == lastSaved) {
            return
        }
        viewModelScope.launch {
            withContext(Dispatchers.IO) {
                lastSaved = name
                profileControllerApi?.changeName(ChangeNameDto(name!!))
            }
        }
    }
}