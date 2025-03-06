package com.artem.tusaandroid.app.beauty

import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.material3.TextField
import androidx.compose.material3.TextFieldDefaults
import androidx.compose.runtime.Composable
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.runtime.getValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.lerp
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.input.ImeAction
import com.artem.tusaandroid.R

@Composable
fun SearchBar(
    modifier: Modifier,
    onSearch: (String) -> Unit
) {
    var query by remember { mutableStateOf("") }
    val textInputColor = lerp(MaterialTheme.colorScheme.surface, Color.White, 0.15f)

    TextField(
        value = query,
        onValueChange = { query = it },
        modifier = modifier,
        placeholder = { Text("Поиск GIF") },
        trailingIcon = {
            IconButton(
                onClick = {
                    onSearch(query)
                }
            ) {
                Icon(painter = painterResource(id = R.drawable.search), contentDescription = "Поиск")
            }
        },
        keyboardOptions = KeyboardOptions(imeAction = ImeAction.Search),
        keyboardActions = KeyboardActions(onSearch = { onSearch(query) }),
        colors = TextFieldDefaults.colors(
            cursorColor = MaterialTheme.colorScheme.primary,
            focusedContainerColor = textInputColor,
            disabledContainerColor = textInputColor,
            unfocusedContainerColor = textInputColor,
            errorContainerColor = textInputColor,
            focusedIndicatorColor = Color.Transparent,
            unfocusedIndicatorColor = Color.Transparent
        ),
    )
}