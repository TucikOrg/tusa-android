package com.artem.tusaandroid.app.admin

import com.artem.tusaandroid.app.IsPreviewViewModel
import com.artem.tusaandroid.app.dialog.AppDialogState
import com.artem.tusaandroid.cropper.CropperState

class ForAdminUserAvatarViewModelPreview : ForAdminUserAvatarViewModel(
    null, null,
    CropperState(AppDialogState())
), IsPreviewViewModel
