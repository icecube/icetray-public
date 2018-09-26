#ifndef I3_SHOVEL_TEXT_TEXTURES_H
#define I3_SHOVEL_TEXT_TEXTURES_H

#include <QPixmap>
#include <QFont>

/** Make a pixmap of some text.  A message and a text color are required.
 * Size will be inferred based on the font and the scaling factor
 * Optionally, the background color can be provided; the true background of the pixmap
 * will be set to bg_color with an alpha value of 0.  Antialiased text will thus blend cleanly
 * into the background color.
 */
QPixmap makeStringPixmap( const std::string& message, QColor color, QColor bg_color = QColor(),
                          QFont font = QFont(), double scaling_factor_=1. );

#endif /* I3_SHOVEL_TEXT_TEXTURES_H */
