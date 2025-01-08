package com.artem.tusaandroid.app.map

import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.RequestTile
import com.artem.tusaandroid.app.MeAvatarState
import com.artem.tusaandroid.app.avatar.AvatarState
import com.artem.tusaandroid.app.selected.SelectedState
import com.artem.tusaandroid.app.systemui.SystemUIState
import com.artem.tusaandroid.location.LastLocationState
import com.artem.tusaandroid.location.LocationsState
import com.artem.tusaandroid.socket.SocketListener
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
open class MapViewModel @Inject constructor(
    val lastLocationState: LastLocationState?,
    val meAvatarState: MeAvatarState?,
    val socketListener: SocketListener?,
    val avatarState: AvatarState?,
    val selectedState: SelectedState?,
    val locationsState: LocationsState?,
    val systemUIState: SystemUIState?,
): ViewModel()

