package com.artem.tusaandroid.app.action.auth

import androidx.compose.material3.ElevatedButton
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext

@Composable
fun CredentialsManagerAuth(modifier: Modifier = Modifier, model: CredentialsManagerAuthViewModel) {
    val activityContext = LocalContext.current
    ElevatedButton(
        modifier = modifier,
        onClick = {
            model.enter(activityContext)
        }
    ) {
        Text("Войти")
    }
}