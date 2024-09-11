package com.artem.tusaandroid.app.login

import android.content.Context
import android.content.Intent
import android.net.Uri
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.AnnotatedString
import androidx.compose.ui.text.SpanStyle
import androidx.compose.ui.text.buildAnnotatedString
import androidx.compose.ui.text.style.TextDecoration
import androidx.compose.ui.text.withStyle
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.AppVariables
import com.artem.tusaandroid.api.AuthenticationControllerApi
import com.artem.tusaandroid.app.action.MainActionFabViewModel
import com.artem.tusaandroid.model.SendCodeDto
import com.artem.tusaandroid.requests.CustomTucikEndpoints
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import org.openapitools.client.infrastructure.ClientException
import javax.inject.Inject

@HiltViewModel
open class InputPhoneViewModel @Inject constructor(
    private val appVariables: AppVariables?,
    private val authenticationControllerApi: AuthenticationControllerApi?,
    private val customTucikEndpoints: CustomTucikEndpoints?
): ViewModel() {

    val legalDocumentsAnnotated = makeLegalDocumentsText()

    fun sendCodeToPhone(phone: String, mainActionFabViewModel: MainActionFabViewModel) {
        appVariables?.savePhone(phone)
        viewModelScope.launch {
            withContext(Dispatchers.IO) {
                try {
                    authenticationControllerApi?.sendCode(SendCodeDto(phone))
                    mainActionFabViewModel.stage = MainActionStage.INPUT_SMS
                } catch (clientException: ClientException) {
                    clientException.printStackTrace()
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

    fun openLegalDocumentsInBrowser(context: Context) {
        legalDocumentsAnnotated.getStringAnnotations("legal_docs", 0, legalDocumentsAnnotated.length).firstOrNull().let {
            val url = customTucikEndpoints?.makeToLegalDocuments()
            val intent = Intent(Intent.ACTION_VIEW, Uri.parse(url))
            context.startActivity(intent)
        }
    }
}