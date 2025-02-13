package com.artem.tusaandroid.location

import androidx.compose.foundation.layout.size
import androidx.compose.material3.FloatingActionButton
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.R

@Composable
fun MoveToMyLocationFab(modifier: Modifier, model: MoveToMyLocationViewModel) {
//    FloatingActionButton(
//        onClick = {
//            model.moveToMe()
//        },
//        modifier = modifier
//    ) {
//        Icon(painter = painterResource(id = R.drawable.my_location), contentDescription = "Приблизить ко мне")
//    }

    IconButton(
        modifier = modifier,
        onClick = {
            model.moveToMe()
        },
    ) {
        Icon(
            modifier = Modifier.size(40.dp),
            painter = painterResource(id = R.drawable.my_location, ),
            contentDescription = "Приблизить ко мне",
        )
    }
}