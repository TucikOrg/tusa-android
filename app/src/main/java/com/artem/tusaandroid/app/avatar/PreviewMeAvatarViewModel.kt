package com.artem.tusaandroid.app.avatar

import com.artem.tusaandroid.app.IsPreviewViewModel
import com.artem.tusaandroid.app.MeAvatarState
import com.artem.tusaandroid.app.profile.PreviewProfileState
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.cropper.CropperState

class PreviewMeAvatarViewModel: MeAvatarViewModel(
    MeAvatarState(PreviewProfileState(), AvatarState(null)), null, CropperState.preview()
), IsPreviewViewModel
