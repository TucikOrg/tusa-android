package com.artem.tusaandroid.app

data class InputPhoneModalResult (
    val dismissed: Boolean,
    val inputPhone: String,
    var ok: Boolean
) {
    companion object {
        fun dismissed(): InputPhoneModalResult {
            return InputPhoneModalResult(dismissed = true, inputPhone = "", ok = false)
        }

        fun ok(inputPhone: String): InputPhoneModalResult {
            return InputPhoneModalResult(dismissed = false, inputPhone = inputPhone, ok = true)
        }

        fun error(): InputPhoneModalResult {
            return InputPhoneModalResult(dismissed = false, inputPhone = "", ok = false)
        }
    }
}