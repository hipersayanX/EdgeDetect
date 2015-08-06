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

inline int gradient(int sumX, int sumY, GradienType gradientType)
{
    switch (gradientType) {
        case GradienTypeX:
            return qAbs(sumX);

        case GradienTypeY:
            return qAbs(sumY);

        case GradienTypeMod:
            return sqrt(sumX * sumX + sumY * sumY);

        case GradienTypeSum:
            return qAbs(sumX) + qAbs(sumY);

        default:
            break;
    }

    return 0;
}

inline QVector<qreal> edgeKernel(int radius,
                                 qreal sigma,
                                 qreal scaleXY,
                                 qreal scaleW,
                                 bool axysY=false,
                                 bool round=false,
                                 int *kl=NULL)
{
    int kw = 2 * radius + 1;
    QVector<qreal> kernel(kw * kw);

    for (int j = 0; j < kw; j++)
        for (int i = 0; i < kw; i++) {
            qreal x = scaleXY * (i - radius);
            qreal y = scaleXY * (j - radius);

            qreal k = - scaleW * (axysY? y: x) / (pow(sigma, 3) * sqrt(2 * M_PI));
            qreal sigma2 = -2 * sigma * sigma;
            qreal weight = k * exp((x * x + y * y) / sigma2);
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
    qreal sigma = 1;
    qreal scaleXY = 1;
    qreal scaleW = 1;
    bool round = false;

    // Create gaussian denoise kernel.
    int kw;
    QVector<qreal> kernelX = edgeKernel(radius, sigma, scaleXY, scaleW, false, round, &kw);
    QVector<qreal> kernelY = edgeKernel(radius, sigma, scaleXY, scaleW, true, round, &kw);

    for (int y = 0; y < inImage.height(); y++) {
        const QRgb *iLine = (const QRgb *) inImage.constScanLine(y);
        QRgb *oLine = (QRgb *) outImage.scanLine(y);

        for (int x = 0; x < inImage.width(); x++) {
            qreal sumX = 0;
            qreal sumY = 0;

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
                    sumX += kernelX[pos] * pixel;
                    sumY += kernelY[pos] * pixel;
                }
            }

            int grad = gradient(sumX, sumY, gradientType);
            quint8 c = qBound(0, grad, 255);
            oLine[x] = qRgba(c, c, c, qAlpha(iLine[x]));
        }
    }

    outImage.save("edge.png");

    return EXIT_SUCCESS;
}
