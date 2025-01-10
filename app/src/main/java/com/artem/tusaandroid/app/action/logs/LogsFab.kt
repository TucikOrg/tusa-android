package com.artem.tusaandroid.app.action.logs

import androidx.compose.foundation.background
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.safeDrawing
import androidx.compose.foundation.layout.windowInsetsPadding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.FloatingActionButton
import androidx.compose.material3.Icon
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.ModalBottomSheetProperties
import androidx.compose.material3.Text
import androidx.compose.material3.rememberModalBottomSheetState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.R
import com.artem.tusaandroid.app.systemui.IsLightGlobal
import androidx.compose.runtime.getValue
import androidx.compose.runtime.setValue
import androidx.compose.ui.graphics.Color
import java.time.format.DateTimeFormatter
import java.util.Locale

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun LogsFab(modifier: Modifier, model: LogsFabViewModel) {
    var showModal by remember { mutableStateOf(false) }

    if (showModal) {
        val sheetState = rememberModalBottomSheetState(skipPartiallyExpanded = true)
        ModalBottomSheet(
            sheetState = sheetState,
            shape = RoundedCornerShape(topEnd = 10.dp, topStart = 10.dp),
            onDismissRequest = {
                showModal = false
            },
            modifier = Modifier.windowInsetsPadding(WindowInsets.safeDrawing),
            properties = ModalBottomSheetProperties(
                isAppearanceLightStatusBars = !IsLightGlobal.isLight
            ),
        ) {
            val scrollState = rememberScrollState()
            val logs = model.getLogs()!!
            val sdf = DateTimeFormatter.ofPattern("HH:mm:ss", Locale.ENGLISH)

            LazyColumn(
                modifier = Modifier.horizontalScroll(scrollState).fillMaxSize()
            ) {
                items(logs.value.size) { index ->
                    val logRow = logs.value[index]
                    val time = logRow.time.format(sdf)
                    Row {
                        Text(
                            modifier = Modifier.background(Color.Gray),
                            text = time
                        )
                        if (logRow.reason.isNotEmpty())
                            Text(
                                modifier = Modifier.background(Color.Green),
                                text = logRow.reason
                            )
                        Text(" | " + logRow.message)
                    }
                }
            }
        }
    }

    FloatingActionButton(
        onClick = {
            showModal = true
        },
        modifier = modifier
    ) {
        Icon(painter = painterResource(id = R.drawable.history), contentDescription = "app logs")
    }
}