package com.artem.tusaandroid.app.profile

import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.height
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.socket.SendMessage
import com.artem.tusaandroid.socket.SocketBinaryMessage
import com.artem.tusaandroid.socket.SocketListener
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.serialization.ExperimentalSerializationApi
import kotlinx.serialization.cbor.Cbor
import kotlinx.serialization.encodeToByteArray
import javax.inject.Inject

@HiltViewModel
open class ProfileCardViewModel @Inject constructor(
    private val profileState: ProfileState?,
    private val socketListener: SocketListener?
): ViewModel() {
    private var showModal = mutableStateOf(false)

    fun getName(): MutableState<String> {
        return profileState?.getName()?: mutableStateOf("Name")
    }

    fun getUniqueName(): MutableState<String> {
        return profileState?.getUniqueName()?: mutableStateOf("unique")
    }

    open fun getPhone(): MutableState<String> {
        return profileState?.getPhone()?: mutableStateOf("Phone")
    }

    @Composable
    fun ShowTitle() {
        Text(
            modifier = Modifier,
            text = getTitle(),
            style = MaterialTheme.typography.headlineSmall,
            fontWeight = FontWeight.Bold,
            color = MaterialTheme.colorScheme.primary
        )
    }

    @Composable
    fun ShowUnderTitleLineOne() {
        val uniqueNameValue = profileState?.getUniqueName()?.value
        if (uniqueNameValue?.isNotEmpty() == true && getTitle() != uniqueNameValue) {
            Spacer(modifier = Modifier.height(3.dp))
            Text(
                text = "@$uniqueNameValue",
                style = MaterialTheme.typography.bodySmall,
                color = Color.Gray
            )
        }
    }

    @Composable
    fun ShowUnderTitleLineTwo() {
        val phoneValue = getPhone().value
        if (phoneValue.isNotEmpty() && getTitle() != phoneValue) {
            Spacer(modifier = Modifier.height(3.dp))
            Text(
                text = phoneValue,
                style = MaterialTheme.typography.bodySmall,
                color = Color.Gray
            )
        }
    }

    fun getTitle(): String {
        val nameValue = profileState?.getName()?.value
        if (nameValue?.isNotEmpty() == true) {
            return nameValue
        }

        val uniqueNameValue = profileState?.getUniqueName()?.value
        if (uniqueNameValue?.isNotEmpty() == true) {
            return uniqueNameValue
        }

        return profileState?.getPhone()?.value?: "Phone"
    }

    fun getShowModal(): Boolean {
        return showModal.value
    }

    fun openModal() {
        showModal.value = true
    }

    fun closeModal() {
        showModal.value = false
    }

    fun dismiss() {
        showModal.value = false
        saveName()
    }

    fun saveName() {
        val nameValue = profileState?.getName()?.value?: ""
        profileState?.changeName(nameValue)
    }

    fun saveUniqueName() {
        val uniqueNameValue = profileState?.getUniqueName()?.value?: ""
        profileState?.changeUniqueName(uniqueNameValue)
    }
}