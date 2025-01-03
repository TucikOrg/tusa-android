package com.artem.tusaandroid.app.profile

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.ElevatedButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.TucikViewModel
import com.artem.tusaandroid.app.action.MainActionFabViewModel
import com.artem.tusaandroid.app.action.PreviewMainActionFabViewModel
import com.artem.tusaandroid.app.avatar.MeAvatar
import com.artem.tusaandroid.app.avatar.PreviewMeAvatarViewModel
import com.artem.tusaandroid.isPreview

@Preview
@Composable
fun ProfileCardPreview() {
    ProfileCard(PreviewProfileCardViewModel(), PreviewMainActionFabViewModel())
}

@Composable
fun ProfileCard(model: ProfileCardViewModel, mainModel: MainActionFabViewModel) {
    if (model.getShowModal()) {
        MyProfileModal(model, mainModel)
    }

    ElevatedButton(
        modifier = Modifier
            .fillMaxWidth(),
        shape = RoundedCornerShape(10.dp),
        onClick = {
            model.openModal()
        }
    ) {
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(vertical = 6.dp, horizontal = 6.dp)
                .padding(start = 21.dp),
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
                    .size(70.dp)
                    .padding(0.dp),
                model = TucikViewModel(preview = model.isPreview(), previewModel = PreviewMeAvatarViewModel())
            )
        }
    }
}
