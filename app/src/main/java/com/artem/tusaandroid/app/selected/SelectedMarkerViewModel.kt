package com.artem.tusaandroid.app.selected

import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.app.action.friends.FriendsState
import com.artem.tusaandroid.location.LocationsState
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject


@HiltViewModel
open class SelectedMarkerViewModel @Inject constructor(
    val selectedState: SelectedState?,
    val locationsState: LocationsState?,
    val friendsState: FriendsState?
): ViewModel() {

}