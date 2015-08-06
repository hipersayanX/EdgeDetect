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

inline QVector<qreal> edgeKernel(int radius,
                                 qreal sigma,
                                 qreal scaleW,
                                 qreal offsetW,
                                 bool round=false,
                                 int *kl=NULL)
{
    int kw = 2 * radius + 1;
    QVector<qreal> kernel(kw * kw);

    for (int j = 0; j < kw; j++)
        for (int i = 0; i < kw; i++) {
            qreal x = i - radius;
            qreal y = j - radius;

            qreal k = scaleW * (x * x + y * y - sigma * sigma) / (pow(sigma, 5) * sqrt(2 * M_PI));
            qreal sigma2 = -2 * sigma * sigma;
            qreal weight = k * exp((x * x + y * y) / sigma2) + offsetW;
            kernel[i + j * kw] = round? qRound(weight): weight;
        }

    *kl = kw;

    return kernel;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Q_UNUSED(a)

    QImage inImage("lena.png");
    inImage = inImage.convertToFormat(QImage::Format_RGB32);
    QImage outImage(inImage.size(), inImage.format());

    // Here we configure the edge detector parameters.
    int radius = 1;
    qreal sigma = 0.21;
    qreal scaleW = 0.2;
    qreal offsetW = 1;
    bool round = true;

    // Create gaussian denoise kernel.
    int kw;
    QVector<qreal> kernel = edgeKernel(radius, sigma, scaleW, offsetW, round, &kw);

    for (int y = 0; y < inImage.height(); y++) {
        const QRgb *iLine = (const QRgb *) inImage.constScanLine(y);
        QRgb *oLine = (QRgb *) outImage.scanLine(y);

        for (int x = 0; x < inImage.width(); x++) {
            qreal sum = 0;

            // Apply kernel.
            for (int j = 0, pos = 0; j < kw; j++) {
                const QRgb *line = (const QRgb *) inImage.constScanLine(y + j - radius);

                if (y + j < radius
                    || y + j >= radius + inImage.height())
                    continue;

                for (int i = 0; i < kw; i++, pos++) {
                    if (x + i < radius
                        || x + i >= radius + inImage.width())
                        continue;

                    quint8 pixel = qGray(line[x + i - radius]);
                    sum += kernel[pos] * pixel;
                }
            }

            quint8 c = qBound(0., qAbs(sum), 255.);
            oLine[x] = qRgba(c, c, c, qAlpha(iLine[x]));
        }
    }

    outImage.save("edge.png");

    return EXIT_SUCCESS;
}
