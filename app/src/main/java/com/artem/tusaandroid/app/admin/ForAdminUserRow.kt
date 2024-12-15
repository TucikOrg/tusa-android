package com.artem.tusaandroid.app.admin

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.ElevatedButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.TucikViewModel
import com.artem.tusaandroid.app.action.friends.PreviewFriendAvatarViewModel
import com.artem.tusaandroid.dto.User
import com.artem.tusaandroid.isPreview


@Composable
fun ForAdminUserRow(model: ForAdminUserRowViewModel, user: User, showProfileModel: ForAdminUserProfileViewModel) {
    LaunchedEffect(Unit) {
        model.checkProfileState(user)
    }

    val name = model.getName(user.id)
    ElevatedButton(
        modifier = Modifier.fillMaxSize(),
        shape = RoundedCornerShape(0.dp),
        onClick = {
            showProfileModel.openModal(user)
        }
    ) {
        Row(
            modifier = Modifier.fillMaxSize(),
            verticalAlignment = Alignment.CenterVertically,
        ) {
            ForAdminUserAvatar(
                modifier = Modifier
                    .size(70.dp)
                    .padding(0.dp),
                model = TucikViewModel(preview = model.isPreview(), previewModel = PreviewFriendAvatarViewModel()),
                userId = user.id
            )
            Spacer(modifier = Modifier.width(20.dp))
            Column(
                modifier = Modifier.weight(1.0f),
            ) {
                Text(
                    modifier = Modifier,
                    text = name.value,
                    style = MaterialTheme.typography.headlineSmall
                )
                Spacer(modifier = Modifier.height(3.dp))
                if (model.getUniqueName(user.id).value.isNotEmpty()) {
                    Text(
                        text = "@${model.getUniqueName(user.id).value}",
                        style = MaterialTheme.typography.bodySmall,
                        color = Color.Gray
                    )
                }
            }
        }
    }
}