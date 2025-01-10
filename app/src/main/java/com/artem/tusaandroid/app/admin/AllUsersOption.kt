package com.artem.tusaandroid.app.admin

import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.safeDrawing
import androidx.compose.foundation.layout.windowInsetsPadding
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.ElevatedButton
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.ModalBottomSheetProperties
import androidx.compose.material3.Text
import androidx.compose.material3.rememberModalBottomSheetState
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.app.action.AdminFabViewModel
import com.artem.tusaandroid.app.action.AdminFabViewModelPreview
import com.artem.tusaandroid.app.systemui.IsLightGlobal


@Preview
@Composable
fun AllUsersContentPreview() {
    AllUsersContent(AdminFabViewModelPreview())
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun AllUsersContent(model: AdminFabViewModel) {
    if (model.allUsersModal) {
        ModalBottomSheet (
            onDismissRequest = {
                model.allUsersModal = false
            },
            sheetState = rememberModalBottomSheetState(skipPartiallyExpanded = true),
            properties = ModalBottomSheetProperties(
                isAppearanceLightStatusBars = !IsLightGlobal.isLight
            ),
            modifier = Modifier.windowInsetsPadding(WindowInsets.safeDrawing)
        ) {
            ScrollableUsersLazyList(model)
        }
    }

    ElevatedButton(
        modifier = Modifier.fillMaxWidth(),
        shape = RoundedCornerShape(10.dp),
        onClick = {
            model.allUsersModal = true
        }
    ) {
        Text(
            text = "Все пользователи",
            fontWeight = FontWeight.Bold,
            fontSize = MaterialTheme.typography.bodyMedium.fontSize
        )
    }
}