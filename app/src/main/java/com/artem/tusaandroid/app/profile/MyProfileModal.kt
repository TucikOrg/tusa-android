package com.artem.tusaandroid.app.profile

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.navigationBarsPadding
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.safeDrawing
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.layout.windowInsetsPadding
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.material3.ElevatedButton
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.ModalBottomSheetProperties
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.material3.TextField
import androidx.compose.material3.rememberModalBottomSheetState
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalSoftwareKeyboardController
import androidx.compose.ui.res.vectorResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.NativeLibrary
import com.artem.tusaandroid.R
import com.artem.tusaandroid.TucikViewModel
import com.artem.tusaandroid.app.action.MainActionFabViewModel
import com.artem.tusaandroid.app.action.PreviewMainActionFabViewModel
import com.artem.tusaandroid.app.avatar.MeAvatar
import com.artem.tusaandroid.app.avatar.PreviewMeAvatarViewModel
import com.artem.tusaandroid.app.systemui.IsLightGlobal
import com.artem.tusaandroid.isPreview

@Preview
@Composable
fun MyProfileModalPreview() {
    MyProfileModal(PreviewProfileCardViewModel(), PreviewMainActionFabViewModel())
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun MyProfileModal(model: ProfileCardViewModel, mainModel: MainActionFabViewModel) {
    val context = LocalContext.current
    val sheetState = rememberModalBottomSheetState(skipPartiallyExpanded = true)
    ModalBottomSheet(
        sheetState = sheetState,
        shape = RoundedCornerShape(topEnd = 10.dp, topStart = 10.dp),
        onDismissRequest = {
            model.dismiss()
        },
        modifier = Modifier.windowInsetsPadding(WindowInsets.safeDrawing),
        properties = ModalBottomSheetProperties(
            isAppearanceLightStatusBars = !IsLightGlobal.isLight
        ),
    ) {
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(horizontal = 10.dp),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Spacer(modifier = Modifier.height(10.dp))
            Row (
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(horizontal = 10.dp)
            ) {
                Column(
                    modifier = Modifier.weight(1.0f),
                ) {
                    model.ShowTitle()
                    model.ShowUnderTitleLineOne()
                    model.ShowUnderTitleLineTwo()
                }
                MeAvatar(
                    modifier = Modifier
                        .size(90.dp)
                        .padding(0.dp),
                    model = TucikViewModel(preview = model.isPreview(), previewModel = PreviewMeAvatarViewModel())
                )
            }
            Spacer(modifier = Modifier.height(20.dp))
            val keyboard = LocalSoftwareKeyboardController.current
            OutlinedTextField(
                value = model.getName().value,
                onValueChange = {
                    model.getName().value = it
                },
                label = { Text("Введите ваше имя") },
                keyboardActions = KeyboardActions(
                    onDone = {
                        model.saveName()
                        keyboard?.hide()
                    }
                ),
                singleLine = true,
                enabled = true,
                textStyle = MaterialTheme.typography.bodyMedium.copy(textAlign = TextAlign.Start),
                modifier = Modifier.fillMaxWidth()
            )
            Spacer(modifier = Modifier.height(20.dp))
            OutlinedTextField(
                value = model.getUniqueName().value,
                onValueChange = {
                    model.getUniqueName().value = it
                },
                label = { Text("Уникальный никнейм (логин)") },
                keyboardActions = KeyboardActions(
                    onDone = {
                        model.saveUniqueName()
                        keyboard?.hide()
                    }
                ),
                leadingIcon = {
                    Text(
                        text = "@",
                        style = MaterialTheme.typography.headlineSmall,
                        fontWeight = FontWeight.Bold
                    )
                },
                singleLine = true,
                enabled = true,
                textStyle = MaterialTheme.typography.bodyMedium.copy(textAlign = TextAlign.Start),
                modifier = Modifier.fillMaxWidth()
            )
            Spacer(modifier = Modifier.weight(1f))
            ElevatedButton(
                modifier = Modifier.fillMaxWidth(),
                shape = RoundedCornerShape(10.dp),
                onClick = {
                    mainModel.logout(context)
                    model.closeModal()
                }
            ) {
                Text(
                    text = "Выйти",
                    fontWeight = FontWeight.Bold,
                    fontSize = MaterialTheme.typography.bodyMedium.fontSize
                )
                Spacer(modifier = Modifier.width(4.dp))
                Icon(
                    imageVector = ImageVector.vectorResource(id = R.drawable.logout),
                    contentDescription = "Logout",
                    modifier = Modifier.size(16.dp)
                )
            }
            Spacer(modifier = Modifier.height(10.dp))
        }
    }
}


