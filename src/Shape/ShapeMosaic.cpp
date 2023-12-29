#include "ShapeMosaic.h"
#include "../WindowMain.h"
#include "../ToolSub.h"
#include "include/effects/Sk2DPathEffect.h"
#include "include/core/SkBlurTypes.h"
#include "include/effects/SkBlurMaskFilter.h"
#include "include/core/SkPathMeasure.h"
#include "include/core/SkColor.h"
#include "include/core/SkStream.h"
#include "include/encode/SkPngEncoder.h"

ShapeMosaic::ShapeMosaic(const int &x, const int &y) : ShapeBase(x, y)
{    
    initParams();
}

ShapeMosaic::~ShapeMosaic()
{
}

void samplePathCoordinates(const SkPath& path, int numSamples) {
    // ���� SkPathMeasure
    
}

bool ShapeMosaic::OnPaint(SkCanvas *canvas)
{

    auto win = WindowMain::get();
    sk_sp<SkImage> img = win->surfaceFront->makeImageSnapshot();
    win->surfaceBack->getCanvas()->drawImage(img, 0, 0);
    float size = 10.f;
    SkPathMeasure pathMeasure(path, false);
    float length = pathMeasure.getLength();
    SkPoint point;
    SkRect rect;
    int tempWidth = strokeWidth / 2;
    SkPaint paint;
    paint.setColor(SK_ColorRED);
    for (float distance = 0; distance < length; distance += size) {
        if (pathMeasure.getPosTan(distance, &point, nullptr)) {
            for (int x = point.fX - tempWidth; x <= point.fX + tempWidth; x += size) {
                for (int y = point.fY - tempWidth; y <= point.fY + tempWidth; y += size) {
                    rect.setXYWH(x, y, size, size);
                    paint.setColor(getMosaicRectColor(rect));
                    canvas->drawRect(rect, paint);
                }                
            }
        }
    }
    paint.setAntiAlias(true);
    paint.setStroke(true);
    paint.setStrokeWidth(strokeWidth);
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
    paint.setStrokeJoin(SkPaint::kRound_Join);
    paint.setBlendMode(SkBlendMode::kClear);
    path.setFillType(SkPathFillType::kInverseWinding);
    canvas->drawPath(path, paint);
    return false;
}

bool ShapeMosaic::OnMouseDown(const int &x, const int &y)
{
    path.moveTo(x, y);
    return false;
}

bool ShapeMosaic::OnMouseUp(const int &x, const int &y)
{
    return false;
}

bool ShapeMosaic::OnMouseMove(const int &x, const int &y)
{
    return false;
}

bool ShapeMosaic::OnMoseDrag(const int &x, const int &y)
{
    isWip = false;
    path.lineTo(x, y);
    WindowMain::get()->Refresh();
    return false;
}

SkColor ShapeMosaic::getMosaicRectColor(const SkRect& rect)
{ 
    auto win = WindowMain::get();
    SkColor4f colorSum = { 0, 0, 0, 0 };
    int count{ 0 };
    for (size_t x = rect.left(); x < rect.right(); x+=2)
    {
        for (size_t y = rect.top(); y < rect.bottom(); y+=2)
        {
            auto currentColor = win->pixBase->getColor4f(x, y);
            colorSum.fR += currentColor.fR;
            colorSum.fG += currentColor.fG;
            colorSum.fB += currentColor.fB;
            count++;
        }
    }
    colorSum.fR /= count;
    colorSum.fG /= count;
    colorSum.fB /= count;
    colorSum.fA = 255;
    return colorSum.toSkColor();
}

void ShapeMosaic::initParams()
{
    HoverIndex = 4;
    path.moveTo(startX, startY);
    auto tool = ToolSub::get();
    stroke = !tool->getFill();
    if (stroke)
    {
        auto stroke = tool->getStroke();
        if (stroke == 1)
        {
            strokeWidth = 36;
        }
        else if (stroke == 2)
        {
            strokeWidth = 66;
        }
        else
        {
            strokeWidth = 96;
        }
    }
}