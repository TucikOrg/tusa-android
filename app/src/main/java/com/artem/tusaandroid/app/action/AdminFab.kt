package com.artem.tusaandroid.app.action

import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.navigationBarsPadding
import androidx.compose.foundation.layout.safeDrawing
import androidx.compose.foundation.layout.windowInsetsPadding
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.FloatingActionButton
import androidx.compose.material3.Icon
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.ModalBottomSheetProperties
import androidx.compose.material3.rememberModalBottomSheetState
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.NativeLibrary
import com.artem.tusaandroid.R
import com.artem.tusaandroid.app.admin.AdminActionsInModal
import com.artem.tusaandroid.app.systemui.IsLightGlobal

@Preview
@Composable
fun AdminFabPreview() {
    AdminFab(Modifier, AdminFabViewModelPreview())
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun AdminFab(modifier: Modifier, model: AdminFabViewModel) {
    if (model.showModal) {
        val sheetState = rememberModalBottomSheetState(skipPartiallyExpanded = true)
        ModalBottomSheet(
            sheetState = sheetState,
            shape = RoundedCornerShape(topEnd = 10.dp, topStart = 10.dp),
            onDismissRequest = {
                model.showModal = false
            },
            properties = ModalBottomSheetProperties(
                isAppearanceLightStatusBars = !IsLightGlobal.isLight
            ),
            modifier = Modifier.windowInsetsPadding(WindowInsets.safeDrawing)
        ) {
            AdminActionsInModal(model)
        }
    }

    FloatingActionButton(
        onClick = {
            model.showModal = true
        },
        modifier = modifier
    ) {
        Icon(painter = painterResource(id = R.drawable.admin_panel_settings), contentDescription = "Admin settings")
    }
}
