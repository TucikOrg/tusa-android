package com.artem.tusaandroid.app.action

import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.navigationBarsPadding
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.FloatingActionButton
import androidx.compose.material3.Icon
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.rememberModalBottomSheetState
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.R
import com.artem.tusaandroid.TucikViewModel
import com.artem.tusaandroid.app.login.InputUniqueName
import com.artem.tusaandroid.app.login.InputPhone
import com.artem.tusaandroid.app.login.InputSMS
import com.artem.tusaandroid.app.login.MainActionStage
import com.artem.tusaandroid.app.login.PreviewInputPhoneModel
import com.artem.tusaandroid.app.login.PreviewInputSMSModel
import com.artem.tusaandroid.app.profile.PreviewProfileCardViewModel
import com.artem.tusaandroid.app.profile.ProfileCard
import com.artem.tusaandroid.isPreview
import com.artem.tusaandroid.location.LocationSetupCard
import com.artem.tusaandroid.location.PreviewLocationSetupCardViewModel

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun MainActionFab(modifier: Modifier, model: MainActionFabViewModel) {
    if (model.showModal) {
        val sheetState = rememberModalBottomSheetState(skipPartiallyExpanded = true)
        ModalBottomSheet(
            sheetState = sheetState,
            shape = RoundedCornerShape(10.dp),
            onDismissRequest = {
                model.showModal = false
            },
            modifier = Modifier.navigationBarsPadding()
        ) {
            when(model.stage) {
                MainActionStage.START -> {}
                MainActionStage.INPUT_PHONE -> {
                    InputPhone(
                        model = TucikViewModel(preview = model.isPreview(), previewModel = PreviewInputPhoneModel()),
                        rootModel = model
                    )
                }
                MainActionStage.INPUT_SMS -> {
                    InputSMS(
                        model = TucikViewModel(preview = model.isPreview(), previewModel = PreviewInputSMSModel()),
                        rootModel = model
                    )
                }
                MainActionStage.INPUT_UNIQUE_NAME -> {
                    InputUniqueName(
                        model = TucikViewModel(preview = model.isPreview(), previewModel = PreviewProfileCardViewModel()),
                        rootModel = model
                    )
                }
                MainActionStage.END -> {}
                MainActionStage.PROFILE -> {
                    MainActionInModal(model)
                }
            }
        }
    }

    FloatingActionButton(
        onClick = {
           model.showModal = true
        },
        modifier = modifier
    ) {
        if (model.authenticationState?.authenticated == true) {
            Icon(painter = painterResource(id = R.drawable.person), contentDescription = "Profile")
        } else {
            Icon(painter = painterResource(id = R.drawable.login), contentDescription = "Login")
        }
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
        Spacer(modifier = Modifier.height(5.dp))
        LocationSetupCard(
            model = TucikViewModel(preview = model.isPreview(), previewModel = PreviewLocationSetupCardViewModel())
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