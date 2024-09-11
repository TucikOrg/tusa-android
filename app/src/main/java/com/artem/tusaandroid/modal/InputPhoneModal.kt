package com.artem.tusaandroid.modal

import android.content.Intent
import android.net.Uri
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.Text
import androidx.compose.material3.rememberModalBottomSheetState
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.hilt.navigation.compose.hiltViewModel
import com.artem.tusaandroid.app.InputPhoneModalResult
import com.artem.tusaandroid.app.InputPhoneModalViewModel
import com.artem.tusaandroid.phone.ru.InputRuPhoneNumber

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun InputPhoneModal(inputPhoneModalViewModel: InputPhoneModalViewModel, onResult: (InputPhoneModalResult) -> Unit) {
    inputPhoneModalViewModel.onResult = onResult
    val sheetState = rememberModalBottomSheetState(skipPartiallyExpanded = true)
    ModalBottomSheet(
        sheetState = sheetState,
        shape = RoundedCornerShape(10.dp),
        onDismissRequest = { inputPhoneModalViewModel.dismiss() },
        windowInsets = WindowInsets(0.dp, 40.dp, 0.dp, 0.dp),
    ) {
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(horizontal = 10.dp),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Text(
                text = "Укажите телефон",
                style = MaterialTheme.typography.headlineSmall,
                fontWeight = FontWeight.Bold
            )

            Text(
                text = "Мы отправим вам смс с кодом для входа",
            )

            Spacer(modifier = Modifier.height(40.dp))
            InputRuPhoneNumber(30.0f) { inputPhone ->
                inputPhoneModalViewModel.sendCode(inputPhone)
            }
            Spacer(modifier = Modifier.height(11.dp))

            val context = LocalContext.current
            Text(
                text = inputPhoneModalViewModel.legalDocumentsAnnotated,
                style = MaterialTheme.typography.bodySmall,
                color = Color.Gray,
                textAlign = TextAlign.Center,
                modifier = Modifier.clickable {
                    inputPhoneModalViewModel.openLegalDocumentsInBrowser(context)
                }
            )
            Spacer(modifier = Modifier.height(21.dp))

            if (inputPhoneModalViewModel.errorText != null) {
                Text(
                    text = inputPhoneModalViewModel.errorText!!,
                    style = MaterialTheme.typography.bodyMedium,
                    fontWeight = FontWeight.Bold,
                    color = Color.Red,
                    textAlign = TextAlign.Center,
                    modifier = Modifier.clickable {
                        inputPhoneModalViewModel.openSupportChannel(context)
                    }
                )
            }
        }
    }
}

@Composable
@Preview
fun InputPhoneModalPreview() {
    InputPhoneModal(inputPhoneModalViewModel = hiltViewModel()) { result ->

    }
}