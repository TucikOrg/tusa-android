package com.artem.tusaandroid.ui.modal

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.size
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ExitToApp
import androidx.compose.material.icons.filled.ExitToApp
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.res.vectorResource
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun SettingsModal(showModal: MutableState<Boolean>) {
    ModalBottomSheet(
        onDismissRequest = { showModal.value = false }
    ) {
        Column {
            TextButton(
                onClick = {

                }
            ) {
                Row {
                    Icon(
                        imageVector = Icons.AutoMirrored.Filled.ExitToApp,
                        contentDescription = "exitToApp",
                        modifier = Modifier.size(18.dp)
                    )
                    Text(text = "Выйти из аккаунта")
                }
            }
        }

    }
}

@Composable
@Preview(showBackground = true)
fun SettingsModalPreview() {
    val showModal = remember {
        mutableStateOf(false)
    }
    SettingsModal(showModal = showModal)
}