package com.artem.tusaandroid.app.profile

import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import com.artem.tusaandroid.app.IsPreviewViewModel

class PreviewProfileCardViewModel: ProfileCardViewModel(null, null), IsPreviewViewModel {
    override fun getPhone(): MutableState<String> {
        return mutableStateOf("Phone")
    }
}