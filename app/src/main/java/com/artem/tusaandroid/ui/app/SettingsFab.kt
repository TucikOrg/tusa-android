package com.artem.tusaandroid.ui.app

import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Settings
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.runtime.Composable
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import com.artem.tusaandroid.ui.modal.SettingsModal

@Composable
fun SettingsFab(modifier: Modifier = Modifier) {
    val showSettingsModal = remember { mutableStateOf(false) }
    if (showSettingsModal.value) {
        SettingsModal(showModal = showSettingsModal)
    }

    IconButton(
        modifier = modifier,
        onClick = {
            showSettingsModal.value = true
        }
    ) {
        Icon(
            modifier = Modifier.fillMaxSize(),
            imageVector = Icons.Default.Settings,
            contentDescription = "Settings",
            tint = Color.White
        )
    }
}