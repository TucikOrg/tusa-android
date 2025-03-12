package com.artem.tusaandroid.app.action

import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.safeDrawing
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.layout.windowInsetsPadding
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.FloatingActionButton
import androidx.compose.material3.Icon
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.ModalBottomSheetProperties
import androidx.compose.material3.rememberModalBottomSheetState
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.hilt.navigation.compose.hiltViewModel
import com.artem.tusaandroid.R
import com.artem.tusaandroid.TucikViewModel
import com.artem.tusaandroid.app.profile.PreviewProfileCardViewModel
import com.artem.tusaandroid.app.profile.ProfileCard
import com.artem.tusaandroid.app.systemui.IsLightGlobal
import com.artem.tusaandroid.isPreview
import com.artem.tusaandroid.location.LocationSetupCard

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun MainActionFab(modifier: Modifier, model: MainActionFabViewModel) {

    if (model.showModal) {
        val sheetState = rememberModalBottomSheetState(skipPartiallyExpanded = true)
        ModalBottomSheet(
            sheetState = sheetState,
            shape = RoundedCornerShape(topStart = 10.dp, topEnd = 10.dp),
            onDismissRequest = {
                model.showModal = false
            },
            modifier = Modifier.windowInsetsPadding(WindowInsets.safeDrawing),
            properties = ModalBottomSheetProperties(
                isAppearanceLightStatusBars = !IsLightGlobal.isLight
            ),
        ) {
            MainActionInModal(model)
        }
    }

    FloatingActionButton(
        onClick = {
            model.showModal = true
        },
        modifier = modifier
    ) {
        Icon(painter = painterResource(id = R.drawable.person), contentDescription = "Profile")
    }
}

@Composable
fun MainActionInModal(model: MainActionFabViewModel) {
    Column(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 10.dp)
    ) {
        ProfileCard(
            model = TucikViewModel(preview = model.isPreview(), previewModel = PreviewProfileCardViewModel()),
            mainModel = model
        )
        Spacer(modifier = Modifier.height(12.dp))
        LocationSetupCard(
            model = hiltViewModel()
        )
        Spacer(modifier = Modifier.height(80.dp))
    }
}

@Preview
@Composable
fun MainActionFabModalContent() {
    MainActionInModal(PreviewMainActionFabViewModel())
}

@Preview
@Composable
fun MainActionModalPreview() {
    Box(
        modifier = Modifier
            .fillMaxSize()
            .padding(0.dp)
    ) {
        MainActionFab(
            modifier = Modifier
                .align(Alignment.BottomCenter)
                .width(120.dp)
                .height(120.dp)
                .padding(16.dp),
            PreviewMainActionFabViewModel()
        )
    }
}