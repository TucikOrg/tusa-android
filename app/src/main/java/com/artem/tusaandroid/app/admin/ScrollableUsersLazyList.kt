package com.artem.tusaandroid.app.admin

import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.rememberLazyListState
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Refresh
import androidx.compose.material3.FloatingActionButton
import androidx.compose.material3.Icon
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.derivedStateOf
import androidx.compose.runtime.remember
import com.artem.tusaandroid.TucikViewModel
import com.artem.tusaandroid.app.action.AdminFabViewModel
import com.artem.tusaandroid.isPreview
import androidx.compose.runtime.getValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp

@Composable
fun ScrollableUsersLazyList(model: AdminFabViewModel) {
    LaunchedEffect(Unit) {
        model.loadMoreUsers()
    }

    val listState = rememberLazyListState()
    val isAtTheEndOfList by remember(listState) {
        derivedStateOf {
            listState.layoutInfo.visibleItemsInfo.lastOrNull()?.index == listState.layoutInfo.totalItemsCount - 1
        }
    }

    LaunchedEffect(isAtTheEndOfList) {
        if (isAtTheEndOfList && model.users.isNotEmpty()) model.loadMoreUsers()
    }

    var showProfileModel: ForAdminUserProfileViewModel = TucikViewModel(
        preview = model.isPreview(),
        previewModel = ForAdminUserProfileViewModelPreview()
    )
    ForAdminUserProfile(
        model = showProfileModel,
    )

    Box(
        modifier = Modifier.fillMaxSize()
    ) {
        LazyColumn(
            state = listState
        ) {
            items(model.users.size, key = { index -> model.users[index].id }) { index ->
                ForAdminUserRow(
                    model = TucikViewModel(model.isPreview(), ForAdminUserRowViewModelPreview()),
                    user = model.users[index],
                    showProfileModel = showProfileModel
                )
            }
        }
        FloatingActionButton(
            modifier = Modifier.align(Alignment.BottomEnd).padding(16.dp),
            onClick = {
                model.restart()
            }
        ) {
            Icon(
                imageVector = Icons.Default.Refresh,
                contentDescription = "Refresh"
            )
        }
    }

}