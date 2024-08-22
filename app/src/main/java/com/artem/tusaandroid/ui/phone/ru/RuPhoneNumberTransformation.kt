package com.artem.tusaandroid.ui.phone.ru

import androidx.compose.ui.text.AnnotatedString
import androidx.compose.ui.text.input.OffsetMapping
import androidx.compose.ui.text.input.TransformedText
import androidx.compose.ui.text.input.VisualTransformation

class RuPhoneNumberTransformation : VisualTransformation {
    override fun filter(text: AnnotatedString): TransformedText {
        val phoneSize = 10
        var phone = if (text.text.length > phoneSize) {
            text.text.substring(0..phoneSize)
        } else {
            text.text
        }

        if (!phone.startsWith("7") && phone.isNotEmpty()) {
            phone = phone.substring(1)
            phone = "7$phone"
        }

        val out = StringBuilder()
        for (i in phone.indices) {
            out.append(phone[i])
            if (i == 1 || i == 4) out.append('-')
        }

        val numberOffsetTranslator = object : OffsetMapping {
            override fun originalToTransformed(offset: Int): Int {
                if (offset <= 2) return offset
                if (offset <= 5) return offset + 1
                if (offset <= 9) return offset + 2
                return 12
            }

            override fun transformedToOriginal(offset: Int): Int {
                if (offset <= 3) return offset
                if (offset <= 7) return offset - 1
                if (offset <= 12) return offset - 2
                return 9
            }
        }

        return TransformedText(AnnotatedString(out.toString()), numberOffsetTranslator)
    }
}