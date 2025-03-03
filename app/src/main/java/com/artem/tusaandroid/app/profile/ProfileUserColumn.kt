package com.artem.tusaandroid.app.profile

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.lerp
import androidx.compose.ui.unit.dp
import androidx.hilt.navigation.compose.hiltViewModel
import java.time.Instant
import java.time.ZoneId
import java.time.format.DateTimeFormatter

@Composable
fun ProfileUserColumn(
    modifier: Modifier,
    name: String,
    uniqueName: String?,
    userId: Long,
    lastOnlineTime: Long?,
    model: ProfileUserColumnViewModel = hiltViewModel()
) {
    Column(
        modifier = modifier,
    ) {
        Text(
            modifier = Modifier,
            text = name,
            style = MaterialTheme.typography.bodyLarge
        )
        Spacer(modifier = Modifier.height(3.dp))
        if (uniqueName != null) {
            Text(
                text = "@$uniqueName",
                style = MaterialTheme.typography.bodySmall,
                color = Color.Gray
            )
        }

        val isOnline = model.isOnlineState(userId)
        if (isOnline.value) {
            Text("В сети",
                style = MaterialTheme.typography.bodySmall,
                color = lerp(Color.Green, Color.Black, 0.2f)
            )
        } else if (lastOnlineTime != null){
            val localDateTime = Instant.ofEpochSecond(lastOnlineTime)
                .atZone(ZoneId.systemDefault())
                .toLocalDateTime()
            val time = DateTimeFormatter.ofPattern("HH:mm MM.dd").format(localDateTime)
            Text("Был(а) в сети $time",
                style = MaterialTheme.typography.bodySmall,
                color = Color.Gray
            )
        }

    }
}