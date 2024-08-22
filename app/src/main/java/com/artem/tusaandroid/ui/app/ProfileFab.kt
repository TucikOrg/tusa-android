package com.artem.tusaandroid.ui.app

import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.navigationBars
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Person
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.FloatingActionButton
import androidx.compose.material3.Icon
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalDensity
import androidx.compose.ui.unit.dp

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun ProfileFab(modifier: Modifier) {
    var showModal by remember { mutableStateOf(false) }

    if (showModal) {
        ModalBottomSheet(
            onDismissRequest = { showModal = false }
        ) {
            val insets = WindowInsets.navigationBars.getBottom(LocalDensity.current)
            MeCard()
            LocationPermissionCard()
            Spacer(modifier = Modifier.height(insets.dp))
        }
    }

    FloatingActionButton(
        onClick = {
            showModal = true
        },
        modifier = modifier
    ) {
        Icon(imageVector = Icons.Default.Person, contentDescription = "avatar upload")
    }
}