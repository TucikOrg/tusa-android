package com.artem.tusaandroid.app.profile

import com.artem.tusaandroid.app.IsPreviewViewModel

class PreviewProfileCardViewModel: ProfileCardViewModel(null, null), IsPreviewViewModel {
    override fun getPhone(): String {
        return "+79772775266"
    }
}