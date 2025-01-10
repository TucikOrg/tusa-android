package com.artem.tusaandroid.app.admin

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.app.action.AdminFabViewModel
import com.artem.tusaandroid.app.action.AdminFabViewModelPreview


@Preview
@Composable
fun AdminActionsPreview() {
    AdminActionsInModal(AdminFabViewModelPreview())
}

@Composable
fun AdminActionsInModal(model: AdminFabViewModel) {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(horizontal = 10.dp)
    ) {
        Text("Для тестирования", style = MaterialTheme.typography.titleMedium)
        Spacer(modifier = Modifier.height(5.dp))
        CreateUserOption(model)
        Spacer(modifier = Modifier.height(5.dp))
        AllUsersContent(model)
    }
}