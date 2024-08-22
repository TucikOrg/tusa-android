package com.artem.tusaandroid.ui.app

import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp

@Composable
fun CharView(text: String, selected: Boolean) {
    val color = if (selected) MaterialTheme.colorScheme.primary else Color.Gray
    val border = if (selected) 3.dp else 1.dp
    Box(
        modifier = Modifier
            .padding(vertical = 4.dp, horizontal = 1.dp)
            .width(30.dp)
            .height(40.dp)
            .background(Color.White)
            .border(border, color, RoundedCornerShape(8.dp))
            .padding(1.dp),
        contentAlignment = Alignment.Center) {
        Text(text)
    }
}