/* EdgeDetect, Implementation of some edge detection filters.
 * Copyright (C) 2015  Gonzalo Exequiel Pedone
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Email   : hipersayan DOT x AT gmail DOT com
 * Web-Site: http://github.com/hipersayanX/EdgeDetect
 */

#include <iostream>
#include <cmath>
#include <QCoreApplication>
#include <QImage>

enum GradienType {
    GradienTypeX,
    GradienTypeY,
    GradienTypeMod,
    GradienTypeSum
};

GradienType gradientType = GradienTypeSum;

inline int gradient(int diffX, int diffY, GradienType gradientType)
{
    switch (gradientType) {
        case GradienTypeX:
            return qAbs(diffX);

        case GradienTypeY:
            return qAbs(diffY);

        case GradienTypeMod:
            return sqrt(diffX * diffX + diffY * diffY);

        case GradienTypeSum:
            return qAbs(diffX) + qAbs(diffY);

        default:
            break;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Q_UNUSED(a)

    QImage inImage("lena.png");
    inImage = inImage.convertToFormat(QImage::Format_RGB32);
    QImage outImage(inImage.size(), inImage.format());

    for (int y = 0; y < inImage.height(); y++) {
        const QRgb *iLine = (const QRgb *) inImage.constScanLine(y);
        const QRgb *iLinePrev = (const QRgb *) inImage.constScanLine(y - 1);
        QRgb *oLine = (QRgb *) outImage.scanLine(y);

        for (int x = 0; x < inImage.width(); x++) {
            int diffX = qGray(iLine[x]);
            int diffY = diffX;

            if (x > 0)
                diffX -= qGray(iLine[x - 1]);

            if (y > 0)
                diffY -= qGray(iLinePrev[x]);

            int grad = gradient(diffX, diffY, gradientType);
            quint8 c = qBound(0, grad, 255);
            oLine[x] = qRgba(c, c, c, qAlpha(iLine[x]));
        }
    }

    outImage.save("edge.png");

    return EXIT_SUCCESS;
}
