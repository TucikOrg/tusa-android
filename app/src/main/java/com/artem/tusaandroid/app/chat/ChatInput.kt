package com.artem.tusaandroid.app.chat

import android.content.Intent
import android.net.Uri
import android.os.Build
import android.os.Bundle
import android.text.InputType
import android.view.inputmethod.EditorInfo
import android.view.inputmethod.InputConnection
import android.widget.EditText
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.PickVisualMediaRequest
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.ExperimentalLayoutApi
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.FilledIconButton
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.material3.TextField
import androidx.compose.material3.TextFieldDefaults
import androidx.compose.runtime.Composable
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.runtime.getValue
import androidx.compose.runtime.setValue
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.ImageBitmap
import androidx.compose.ui.graphics.lerp
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.vectorResource
import coil.compose.AsyncImage
import com.artem.tusaandroid.R

@OptIn(ExperimentalMaterial3Api::class, ExperimentalLayoutApi::class)
@Composable
fun ChatInput(chatViewModel: ChatViewModel) {
    val attaches by chatViewModel.getAttaches()

    Column(
        modifier = Modifier.fillMaxWidth()
    ) {
        InputMessage(chatViewModel)
        if (attaches.isNotEmpty()) {
            Row(
                modifier = Modifier.fillMaxWidth()
                    .background(MaterialTheme.colorScheme.surface)
                    .padding(horizontal = 3.dp, vertical = 5.dp),
                verticalAlignment = Alignment.CenterVertically,
                horizontalArrangement = Arrangement.Start
            ) {
                for (attachUri in attaches) {
                    Box {
                        AsyncImage(
                            model = attachUri.uri,
                            contentDescription = "Selected Image",
                            modifier = Modifier.size(70.dp)
                        )

                        IconButton(
                            onClick = {
                                chatViewModel.removeAttach(attachUri.uri)
                            },
                            modifier = Modifier
                                .align(Alignment.TopEnd)
                                .clip(CircleShape)
                                .background(Color.Red.copy(alpha = 0.7f))
                                .size(20.dp)
                        ) {
                            Icon(
                                modifier = Modifier.size(10.dp),
                                imageVector = ImageVector.vectorResource(id = R.drawable.close),
                                contentDescription = "Delete Image",
                                tint = Color.White
                            )
                        }
                    }
                }
            }
        }
    }

}

@Composable
fun InputMessage(chatViewModel: ChatViewModel) {
    val context = LocalContext.current
    var message by remember { mutableStateOf("") }
    val pickAvatarLauncher = rememberLauncherForActivityResult(
        contract = ActivityResultContracts.PickVisualMedia(),

    ) { uri: Uri? ->
        if (uri == null)
            return@rememberLauncherForActivityResult

        // чтобы uri жил вечно и не умирал после перезапуска приложения
        context.contentResolver.takePersistableUriPermission(
            uri, Intent.FLAG_GRANT_READ_URI_PERMISSION
        )

        chatViewModel.imageUriSelected(uri, context)
    }

    Row(
        modifier = Modifier.fillMaxWidth()
            .background(MaterialTheme.colorScheme.surface)
            .padding(horizontal = 3.dp, vertical = 5.dp),
        verticalAlignment = Alignment.Bottom,
    ) {
        IconButton(
            onClick = {
                pickAvatarLauncher.launch(
                    PickVisualMediaRequest(ActivityResultContracts.PickVisualMedia.ImageOnly)
                )
            }
        ) {
            Icon(
                imageVector = ImageVector.vectorResource(id = R.drawable.clip),
                contentDescription = "Add attachment"
            )
        }
        val textInputColor = lerp(MaterialTheme.colorScheme.surface, Color.White, 0.15f)
        TextField(
            value = message,
            onValueChange = {
                message = it
                chatViewModel.writingMessage(message)
            },
            placeholder = { Text(text = "Сообщение") },
            modifier = Modifier.weight(1f),
            shape = RoundedCornerShape(16.dp), // Rounded corners
            colors = TextFieldDefaults.colors(
                cursorColor = MaterialTheme.colorScheme.primary,
                focusedContainerColor = textInputColor,
                disabledContainerColor = textInputColor,
                unfocusedContainerColor = textInputColor,
                errorContainerColor = textInputColor,
                focusedIndicatorColor = Color.Transparent,
                unfocusedIndicatorColor = Color.Transparent
            ),
            textStyle = MaterialTheme.typography.bodyLarge,
        )
        FilledIconButton (
            onClick = {
                // отправляем сообщение
                chatViewModel.sendMessage(message)

                // очищаем у друга поле "пишет сообщение"
                chatViewModel.writingMessage("")

                // очищаем поле ввода локальное
                message = ""
            },
            modifier = Modifier.padding(4.dp),
        ) {
            Icon(
                imageVector = ImageVector.vectorResource(id = R.drawable.arrow_up),
                contentDescription = "Send message"
            )
        }
    }
}
