package com.artem.tusaandroid.app.admin

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.navigationBarsPadding
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.material3.ElevatedButton
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.material3.rememberModalBottomSheetState
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalSoftwareKeyboardController
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.TucikViewModel
import com.artem.tusaandroid.isPreview

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun ForAdminUserProfile(model: ForAdminUserProfileViewModel) {
    if (model.getShowModal() == false) return

    val sheetState = rememberModalBottomSheetState(skipPartiallyExpanded = true)
    ModalBottomSheet(
        sheetState = sheetState,
        shape = RoundedCornerShape(10.dp),
        onDismissRequest = {
            model.closeModal()
        },
        modifier = Modifier.navigationBarsPadding()
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
                }
                ForAdminUserAvatar(
                    modifier = Modifier
                        .size(90.dp)
                        .padding(0.dp),
                    model = TucikViewModel(preview = model.isPreview(), previewModel = ForAdminUserAvatarViewModelPreview()),
                    userId = model.userId.longValue
                )
            }
            Spacer(modifier = Modifier.height(20.dp))
            val keyboard = LocalSoftwareKeyboardController.current
            OutlinedTextField(
                value = model.getName().value,
                onValueChange = {
                    model.getName().value = it
                },
                label = { Text("Введите имя") },
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
            ElevatedButton(
                modifier = Modifier.fillMaxWidth(),
                onClick = {
                    model.createFriends()
                }
            ) {
                Text("Создать друзей")
            }
            ElevatedButton(
                modifier = Modifier.fillMaxWidth(),
                onClick = {
                    model.createFriendsRequestToMe()
                }
            ) {
                Text("Дать мне запрос в друзья")
            }
            ElevatedButton(
                modifier = Modifier.fillMaxWidth(),
                onClick = {
                    model.fakeUserPosition()
                }
            ) {
                Text("Находиться в центре текущей зоны карты")
            }
            //Spacer(modifier = Modifier.weight(1f))
        }
    }
}