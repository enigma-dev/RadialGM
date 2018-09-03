#ifndef MAIN_H
#define MAIN_H

#include <QString>

// Qt doesn't have a way of getting the default style
// so we store it for later when the user restores
// default settings so we can apply it again
extern QString defaultStyle;

#endif  // MAIN_H
