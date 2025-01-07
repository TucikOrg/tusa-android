package com.artem.tusaandroid.app.action.friends

import androidx.compose.foundation.clickable
import androidx.compose.foundation.interaction.MutableInteractionSource
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.navigationBarsPadding
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.safeDrawing
import androidx.compose.foundation.layout.windowInsetsPadding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.ModalBottomSheetProperties
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.material3.rememberModalBottomSheetState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalFocusManager
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.R
import com.artem.tusaandroid.TucikViewModel
import com.artem.tusaandroid.isPreview

@Composable
@Preview
fun PreviewFindFriendsModal() {
    FindFriendsModal(PreviewFindFriendViewModel())
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun FindFriendsModal(model: FindFriendViewModel) {
    val sheetState = rememberModalBottomSheetState(skipPartiallyExpanded = true)
    ModalBottomSheet(
        shape = RoundedCornerShape(topEnd = 10.dp, topStart = 10.dp),
        sheetState = sheetState,
        onDismissRequest = {
            model.showModal.value = false
        },
        properties = ModalBottomSheetProperties(
            isAppearanceLightStatusBars = false
        ),
        modifier = Modifier
            .windowInsetsPadding(WindowInsets.safeDrawing)

    ) {
        val focusManager = LocalFocusManager.current
        val mutableIndicationSource = remember { MutableInteractionSource()}

        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(horizontal = 10.dp)
                .clickable(
                    indication = null,
                    interactionSource = mutableIndicationSource,
                    onClick = {
                        focusManager.clearFocus()
                    }
                ),
        ) {
            Text(
                modifier = Modifier.fillMaxWidth(),
                text = "Поиск",
                style = MaterialTheme.typography.headlineSmall,
                fontWeight = FontWeight.Bold,
                textAlign = TextAlign.Start
            )
            Spacer(modifier = Modifier.height(10.dp))
            OutlinedTextField(
                value = model.searchQuery.value,
                onValueChange = {
                    model.searchQuery.value = it
                    model.findUser()
                },
                label = { Text("Уникальное имя друга") },
                shape = RoundedCornerShape(16.dp),
                singleLine = true,
                leadingIcon = {
                    Icon(painter = painterResource(id = R.drawable.search), contentDescription = "Friends")
                },
                modifier = Modifier.fillMaxWidth()
            )
            Spacer(modifier = Modifier.height(10.dp))
            if (model.users.value.isEmpty()) {
                Column(
                    modifier = Modifier
                        .weight(1f)
                        .fillMaxWidth()
                        .padding(10.dp),
                    verticalArrangement = Arrangement.Center,
                    horizontalAlignment = Alignment.CenterHorizontally
                ) {
                    Text(
                        text = "Никого не нашли",
                        style = MaterialTheme.typography.bodyLarge,
                        fontWeight = FontWeight.Bold,
                    )
                }
            } else {
                LazyColumn(
                    modifier = Modifier.weight(1f)
                ) {
                    items(model.users.value) { friend ->
                        FriendRowInSearch(
                            model = TucikViewModel(preview = model.isPreview(), previewModel = PreviewFriendRowInSearchViewModel()),
                            friend = friend,
                        )
                        Spacer(modifier = Modifier.height(10.dp))
                    }
                }
            }

            Spacer(modifier = Modifier.height(40.dp))
        }
    }
}