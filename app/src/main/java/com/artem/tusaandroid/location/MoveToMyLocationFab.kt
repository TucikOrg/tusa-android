package com.artem.tusaandroid.location

import androidx.compose.material3.FloatingActionButton
import androidx.compose.material3.Icon
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.painterResource
import com.artem.tusaandroid.R

@Composable
fun MoveToMyLocationFab(modifier: Modifier, model: MoveToMyLocationViewModel) {
    FloatingActionButton(
        onClick = {
            model.moveToMe()
        },
        modifier = modifier
    ) {
        Icon(painter = painterResource(id = R.drawable.my_location), contentDescription = "Приблизить ко мне")
    }
}