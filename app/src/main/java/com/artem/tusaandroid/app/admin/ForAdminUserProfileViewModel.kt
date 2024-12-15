package com.artem.tusaandroid.app.admin

import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.height
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableLongStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.NativeLibrary
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.dto.Profile
import com.artem.tusaandroid.dto.User
import com.artem.tusaandroid.socket.SocketListener
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject


@HiltViewModel
open class ForAdminUserProfileViewModel @Inject constructor(
    private val profileState: ProfileState?,
    private val socket: SocketListener?
): ViewModel() {
    private var showModal = mutableStateOf(false)
    private lateinit var user: User
    var userId = mutableLongStateOf(0L)

    fun getName(): MutableState<String> {
        return profileState?.getName(user.id)?: mutableStateOf("Name")
    }

    fun getUniqueName(): MutableState<String> {
        return profileState?.getUniqueName(user.id)?: mutableStateOf("unique")
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
        var uniqueNameValue = profileState?.getUniqueName(user.id)?.value

        if (uniqueNameValue?.isNotEmpty() == true && getTitle() != uniqueNameValue) {
            Spacer(modifier = Modifier.height(3.dp))
            Text(
                text = "@$uniqueNameValue",
                style = MaterialTheme.typography.bodySmall,
                color = Color.Gray
            )
        }
    }

    fun getTitle(): String {
        val nameValue = profileState?.getName(user.id)?.value
        if (nameValue?.isNotEmpty() == true) {
            return nameValue
        }

        val uniqueNameValue = profileState?.getUniqueName(user.id)?.value
        if (uniqueNameValue?.isNotEmpty() == true) {
            return uniqueNameValue
        }

        return profileState?.getPhone()?.value?: "Phone"
    }

    fun getShowModal(): Boolean {
        return showModal.value
    }

    fun openModal(user: User) {
        userId.longValue = user.id
        this.user = user
        showModal.value = true
    }

    fun closeModal() {
        showModal.value = false
    }

    fun saveName() {
        if (profileState == null) return

        val nameValue = profileState.getName(user.id).value
        profileState.changeName(nameValue, user.id)
    }

    fun saveUniqueName() {
        if (profileState == null) return

        val uniqueNameValue = profileState.getUniqueName(user.id).value
        profileState.changeUniqueName(uniqueNameValue, user.id)
    }

    fun createFriends() {
        if (socket == null) return

        socket.getSendMessage()!!.createFriends(userId.longValue)
    }

    fun createFriendsRequestToMe() {
        if (socket == null) return

        socket.getSendMessage()!!.createRequestToMe(userId.longValue)
    }

    fun fakeUserPosition() {
        if (socket == null) return

        val cameraPos = NativeLibrary.getCameraPos()
        socket.getSendMessage()!!.fakeLocation(cameraPos.latitude, cameraPos.longitude, userId.longValue)
    }
}