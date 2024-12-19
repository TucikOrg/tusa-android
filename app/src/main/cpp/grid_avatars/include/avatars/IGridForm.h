//
// Created by Artem on 19.12.2024.
//

#ifndef TUSA_ANDROID_AVATARS_IGRIDFORM_H
#define TUSA_ANDROID_AVATARS_IGRIDFORM_H

namespace Avatars {
    class IGridForm {
    public:
        virtual bool intersects(IGridForm& form) const = 0;
    };
}



#endif //TUSA_ANDROID_IGRIDFORM_H
