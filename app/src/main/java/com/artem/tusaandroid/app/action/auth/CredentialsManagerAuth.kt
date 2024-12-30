package com.artem.tusaandroid.app.action.auth

import android.content.Context
import android.content.Intent
import android.net.Uri
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.heightIn
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Email
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.CardElevation
import androidx.compose.material3.Checkbox
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.runtime.setValue
import androidx.compose.runtime.getValue
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.res.vectorResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.sp
import com.artem.tusaandroid.R

@Preview
@Composable
fun CredentialsManagerAuthPreview() {
    CredentialsManagerAuth(CredentialsManagerAuthViewModelPreview())
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun CredentialsManagerAuth(model: CredentialsManagerAuthViewModel) {
    val activityContext = LocalContext.current
    val context = LocalContext.current
    var isChecked by remember { mutableStateOf(model.getAgreementState()) }
    val alreadyAccepted = model.getAgreementState()

    Box(
        modifier = Modifier
            .fillMaxSize()
            .background(Color.Transparent)
            .padding(16.dp),
        contentAlignment = Alignment.BottomCenter
    ) {

        if (alreadyAccepted) {
            // Sign in button with Gmail icon
            EnterButton(activityContext, model, isChecked)
            return
        }

        Card(
            shape = RoundedCornerShape(16.dp),
            elevation = CardDefaults.cardElevation(defaultElevation = 4.dp),
            modifier = Modifier
                .fillMaxWidth()
                .heightIn(min = 60.dp) // Min height
        ) {
            Column(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(16.dp),
                horizontalAlignment = Alignment.CenterHorizontally
            ) {

                // Принять пользовательское соглашение
                Row(
                    verticalAlignment = Alignment.CenterVertically,
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(bottom = 16.dp)
                ) {
                    Checkbox(
                        checked = isChecked,
                        onCheckedChange = {
                            isChecked = it
                            if (isChecked) {
                                model.acceptAgreement()
                            }
                        }
                    )
                    Spacer(modifier = Modifier.width(8.dp))
                    Text(
                        text = "Принять пользовательское соглашение",
                        modifier = Modifier.clickable {
                            val intent = Intent(
                                Intent.ACTION_VIEW,
                                Uri.parse("https://tucik.fun/privacy.pdf")
                            )
                            context.startActivity(intent)
                        },
                        fontSize = 14.sp,
                        color = MaterialTheme.colorScheme.primary,
                    )
                }


                // Sign in button with Gmail icon
                EnterButton(activityContext, model, isChecked)
            }
        }
    }
}

@Composable
fun EnterButton(context: Context, model: CredentialsManagerAuthViewModel, isChecked: Boolean) {
    // Sign in button with Gmail icon
    Button(
        onClick = {
            model.enter(context)
        },
        enabled = isChecked,
        modifier = Modifier.fillMaxWidth(),
        colors = ButtonDefaults.buttonColors(
            containerColor = MaterialTheme.colorScheme.primary,
            contentColor = Color.White
        )
    ) {
        Icon(
            imageVector = ImageVector.vectorResource(id = R.drawable.gmail_icon),
            contentDescription = "Gmail Icon",
            modifier = Modifier.size(20.dp),
            tint = Color.Unspecified
        )
        Spacer(modifier = Modifier.width(8.dp))
        Text(
            text = "Войти",
            fontWeight = FontWeight.Bold
        )
    }
}