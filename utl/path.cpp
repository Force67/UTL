
/*
 * UTL : The universal utility library
 *
 * Copyright 2019-2020 Force67.
 * For information regarding licensing see LICENSE
 * in the root of the source tree.
 */

#include "path.h"
#include "base.h"

namespace utl {

std::string get_home_dir();

std::string make_app_path(app_path pathType, optional_path relative) {
    // clearly a bug
    if (pathType == app_path::count)
        BUGCHECK();

    static std::string s_defaultPaths[static_cast<int>(app_path::count)];
    auto& newPath = s_defaultPaths[static_cast<int>(pathType)];

    if (newPath.empty()) {
        switch (pathType) { 
            case app_path::home: {
            newPath = get_home_dir() + UTL_PATH_SEP PRJ_COMPANY UTL_PATH_SEP PRJ_NAME UTL_PATH_SEP;
            break;
            }
            case app_path::self:
            default:
                return make_abs_path(relative);
        }

        if (!exists(newPath))
            make_dir(newPath);
    }

    return !relative ? newPath : newPath + *relative;
}
} // namespace utl