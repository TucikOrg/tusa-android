package com.artem.tusaandroid.app.selected

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.heightIn
import androidx.compose.foundation.layout.imePadding
import androidx.compose.foundation.layout.navigationBarsPadding
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.safeDrawing
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.layout.windowInsetsPadding
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.ElevatedButton
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.ModalBottomSheetProperties
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.material3.rememberModalBottomSheetState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableLongStateOf
import androidx.compose.runtime.setValue
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.res.vectorResource
import androidx.compose.ui.tooling.preview.Preview
import com.artem.tusaandroid.NativeLibrary
import com.artem.tusaandroid.R
import com.artem.tusaandroid.TucikViewModel
import com.artem.tusaandroid.app.action.friends.FriendAvatar
import com.artem.tusaandroid.app.action.friends.FriendAvatarViewModel
import com.artem.tusaandroid.app.action.friends.PreviewFriendAvatarViewModel
import com.artem.tusaandroid.isPreview

@Preview
@Composable
fun SelectedMarkerModalPreview() {
    SelectedMarkerModal(SelectedMarkerViewModelPreview())
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun SelectedMarkerModal(model: SelectedMarkerViewModel) {
    val sheetState = rememberModalBottomSheetState(skipPartiallyExpanded = false)
    var selectedMarker = model.selectedState?.selectedMarker?: remember { mutableLongStateOf(1L) }
    val friend = model.friendsState?.friends?.value?.find { it.id == selectedMarker.value }

    if (selectedMarker.value != 0L) {
        ModalBottomSheet(
            shape = RoundedCornerShape(topEnd = 10.dp, topStart = 10.dp),
            sheetState = sheetState,
            onDismissRequest = {
                selectedMarker.value = 0L
                NativeLibrary.deselectSelectedMarker()
            },
            properties = ModalBottomSheetProperties(
                isAppearanceLightStatusBars = false
            ),
            modifier = Modifier.windowInsetsPadding(WindowInsets.safeDrawing)
        ) {
            Column(
                modifier = Modifier
                    .padding(vertical = 6.dp, horizontal = 6.dp),
            ) {
                Row(
                    modifier = Modifier
                        .fillMaxWidth(),
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    FriendAvatar(
                        modifier = Modifier.padding(10.dp).size(90.dp),
                        model = TucikViewModel(preview = model.isPreview(), previewModel = PreviewFriendAvatarViewModel()),
                        userId = selectedMarker.value
                    )
                    Spacer(
                        modifier = Modifier.width(10.dp)
                    )
                    Column(
                        modifier = Modifier.weight(1f),
                        verticalArrangement = Arrangement.Top
                    ) {
                        Text(
                            text = friend?.name?: "User name",
                            style = MaterialTheme.typography.headlineSmall,
                            fontWeight = FontWeight.Bold,
                            color = MaterialTheme.colorScheme.primary
                        )
                        Text(
                            text = ("@" + (friend?.uniqueName?: "uniquename")),
                            style = MaterialTheme.typography.bodySmall,
                            color = Color.Gray
                        )
                    }
                    IconButton(
                        onClick = {
                            val location = model.locationsState?.getLocation(selectedMarker.value)
                            if (location != null) {
                                NativeLibrary.setCameraPos(location.latitude, location.longitude, 15.0f)
                            }
                        }
                    ) {
                        Icon(
                            painter = painterResource(id = R.drawable.location_on),
                            contentDescription = "Point to marker",
                            tint = MaterialTheme.colorScheme.primary,
                            modifier = Modifier.size(50.dp)
                        )
                    }
                    Spacer(modifier = Modifier.width(20.dp))
                }
                ElevatedButton(
                    modifier = Modifier.fillMaxWidth(),
                    onClick = {
                        model.openChatWithUser(selectedMarker.value)
                    }
                ) {
                    Text("Отправить сообщение")
                    Spacer(modifier = Modifier.width(6.dp))
                    Icon(
                        imageVector = ImageVector.vectorResource(id = R.drawable.chat),
                        contentDescription = "Отправить сообщение",
                    )
                }
            }
        }
    }
}