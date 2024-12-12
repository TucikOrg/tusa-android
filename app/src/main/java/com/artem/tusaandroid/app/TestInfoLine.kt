package com.artem.tusaandroid.app

import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.sp

@Composable
fun TestInfoLine(modifier: Modifier, line: String) {
    Box(modifier = modifier) {
        Column(
            horizontalAlignment = Alignment.End
        ) {
            Text(line,
                color = Color.White,
                fontSize = 9.sp
            )
        }
    }
}