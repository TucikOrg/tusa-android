package com.artem.tusaandroid.socket

import androidx.compose.foundation.layout.Column
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.material3.MaterialTheme
import androidx.compose.ui.Alignment
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp

@Preview
@Composable
fun PreviewConnectionStatus() {
    Box(
        modifier = Modifier.fillMaxSize()
    ) {
        ConnectionStatus(
            connectionStatusViewModel = PreviewConnectionStatusViewModel(),
            modifier = Modifier
                .align(Alignment.TopEnd)
                .padding(4.dp),
        )
    }
}

@Composable
fun ConnectionStatus(connectionStatusViewModel: ConnectionStatusViewModel, modifier: Modifier = Modifier) {
    val state = connectionStatusViewModel.getState()
    Box(modifier = modifier) {
        when (state.value) {
            SocketConnectionStates.OPEN -> {
                Text("соединен",
                    color = MaterialTheme.colorScheme.primary
                )
            }
            SocketConnectionStates.CLOSED -> {
                Text("отключен",
                    color = MaterialTheme.colorScheme.primary
                )
            }
            SocketConnectionStates.FAILED -> {
                Text("ошибка",
                    color = MaterialTheme.colorScheme.primary
                )
            }
            SocketConnectionStates.WAIT_TO_RECONNECT -> {
                Row {
                    Text("подключаюсь",
                        color = MaterialTheme.colorScheme.primary
                    )
                    Spacer(modifier = Modifier.width(5.dp))
                    CircularProgressIndicator(
                        modifier = Modifier
                            .size(15.dp)
                            .align(Alignment.CenterVertically),
                        strokeWidth = 2.dp
                    )
                }
            }
        }
    }
}