//
// Created by Artem on 17.10.2024.
//

#include "Symbol.h"

Symbol::Symbol(unsigned int textureId,
               unsigned int width,
               unsigned int rows,
               FT_Int bitmapLeft,
               FT_Int bitmapTop,
               FT_Pos advance) :
        textureId(textureId),
        width(width),
        rows(rows),
        bitmapLeft(bitmapLeft),
        bitmapTop(bitmapTop),
        advance(advance) { }

Symbol::Symbol() {

}
