package com.artem.tusaandroid.app

import android.content.Context
import android.content.Intent
import android.net.Uri
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.AnnotatedString
import androidx.compose.ui.text.SpanStyle
import androidx.compose.ui.text.buildAnnotatedString
import androidx.compose.ui.text.style.TextDecoration
import androidx.compose.ui.text.withStyle
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.api.AuthenticationControllerApi
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.model.SendCodeDto
import com.artem.tusaandroid.requests.CustomTucikEndpoints
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import org.openapitools.client.infrastructure.ClientException
import javax.inject.Inject

@HiltViewModel
class InputPhoneModalViewModel @Inject constructor(
    private val profileState: ProfileState,
    private val authenticationControllerApi: AuthenticationControllerApi,
    private val customTucikEndpoints: CustomTucikEndpoints
): ViewModel() {
    var errorText by mutableStateOf<AnnotatedString?>(null)
    val legalDocumentsAnnotated = makeLegalDocumentsText()

    lateinit var onResult: (InputPhoneModalResult) -> Unit

    fun dismiss() {
        onResult(InputPhoneModalResult.dismissed())
    }

    fun sendCode(phone: String) {
        profileState.savePhone(phone)
        viewModelScope.launch {
            withContext(Dispatchers.IO) {
                try {
                    authenticationControllerApi.sendCode(SendCodeDto(phone))
                    onResult(InputPhoneModalResult.ok(phone))
                } catch (clientException: ClientException) {
                    clientException.printStackTrace()
                    errorText = buildAnnotatedString {
                        append("Не можем отправить код сейчас. Пожалуйста сообщи в эту ")
                        withStyle(
                            style = SpanStyle(
                                color = Color.Blue,
                                textDecoration = TextDecoration.Underline
                            )
                        ) {
                            append("телеграмм группу :)")
                        }
                    }
                    onResult(InputPhoneModalResult.error())
                }
            }
        }
    }

    private fun makeLegalDocumentsText(): AnnotatedString {
        return buildAnnotatedString {
            append("Нажимая кнопку, вы соглашаетесь с условиями наших ")
            pushStringAnnotation(tag = "legal_docs", annotation = "legal_docs")
            withStyle(
                style = SpanStyle(
                    color = Color.Blue,
                    textDecoration = TextDecoration.Underline
                )
            ) {
                append("юридических документов")
            }
            pop()
        }
    }

    fun openSupportChannel(context: Context) {
        val intent = Intent(Intent.ACTION_VIEW, Uri.parse("https://t.me/tucik_support"))
        context.startActivity(intent)
    }

    fun openLegalDocumentsInBrowser(context: Context) {
        legalDocumentsAnnotated.getStringAnnotations("legal_docs", 0, legalDocumentsAnnotated.length).firstOrNull().let {
            val url = customTucikEndpoints.makeToLegalDocuments()
            val intent = Intent(Intent.ACTION_VIEW, Uri.parse(url))
            context.startActivity(intent)
        }
    }
}