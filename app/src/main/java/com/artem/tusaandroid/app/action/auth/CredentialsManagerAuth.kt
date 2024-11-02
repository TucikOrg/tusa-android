package com.artem.tusaandroid.app.action.auth

import androidx.compose.material3.ElevatedButton
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.credentials.CredentialManager

@Composable
fun CredentialsManagerAuth(modifier: Modifier = Modifier) {
    val context = LocalContext.current
    val credentialManager = CredentialManager.create(context)

    ElevatedButton(
        modifier = modifier,
        onClick = { /*TODO*/ }
    ) {
        Text("Войти")
    }
}