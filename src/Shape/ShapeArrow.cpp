﻿#include "ShapeArrow.h"
#include "../WindowMain.h"
#include "../ToolSub.h"

ShapeArrow::ShapeArrow(const int &x, const int &y) : ShapeBase(x, y)
{
    initParams();
}

ShapeArrow::~ShapeArrow()
{
}

bool ShapeArrow::OnMouseDown(const int &x, const int &y)
{
    hoverX = x;
    hoverY = y;
    return false;
}

bool ShapeArrow::OnMouseUp(const int &x, const int &y)
{
    unsigned size = 10;
    unsigned half = 5;
    draggers[0].setXYWH(startX-half, startY-half, size, size);
    draggers[1].setXYWH(endX-half, endY-half, size, size);
    return false;
}

bool ShapeArrow::OnMouseMove(const int &x, const int &y)
{
    for (size_t i = 0; i < draggers.size(); i++)
    {
        if (draggers[i].contains(x, y))
        {
            HoverIndex = i;
            SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
            if (!showDragger)
            {
                showDragger = true;
                WindowMain::get()->Refresh();
            }
            return true;
        }
    }
    auto flag = isMouseOver(x, y);
    if (flag)
    {
        HoverIndex = 8;
        SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
        if (!showDragger)
        {
            showDragger = true;
            WindowMain::get()->Refresh();
        }
        return true;
    }
    return false;
}

bool ShapeArrow::OnMoseDrag(const int &x, const int &y)
{
    IsWIP = false;
    showDragger = false;
    if (HoverIndex == 0) {
        startX = x;
        startY = y;
        makePath(startX, startY, endX, endY);
    }
    else if(HoverIndex == 1) {
        endX = x;
        endY = y;
        makePath(startX, startY, endX, endY);
    }
    else {
        auto xSpan = x - hoverX;
        auto ySpan = y - hoverY;
        startX += xSpan;
        startY += ySpan;
        endX += xSpan;
        endY += ySpan;
        makePath(startX, startY, endX, endY);
        hoverX = x;
        hoverY = y;
    }
    WindowMain::get()->Refresh();
    return false;
}

bool ShapeArrow::OnPaint(SkCanvas* canvas)
{
    SkPaint paint;
    paint.setAntiAlias(true);
    if (stroke)
    {
        paint.setStroke(true);
        paint.setStrokeWidth(strokeWidth);
    }
    paint.setColor(color);
    canvas->drawPath(path, paint);
    paintDragger(canvas);
    return false;
}

bool ShapeArrow::isMouseOver(const int &x, const int &y)
{
    return path.contains(x,y);
}

void ShapeArrow::paintDragger(SkCanvas *canvas)
{
    if (!showDragger)
    {
        return;
    }
    SkPaint paint;
    paint.setStroke(true);
    paint.setStrokeWidth(1);
    paint.setColor(SK_ColorBLACK);
    for (auto &dragger : draggers)
    {
        canvas->drawRect(dragger, paint);
    }
}

void ShapeArrow::initParams()
{
    HoverIndex = 1;
    auto tool = ToolSub::get();
    stroke = !tool->getFill();
    if (stroke)
    {
        auto stroke = tool->getStroke();
        if (stroke == 1)
        {
            strokeWidth = 4;
        }
        else if (stroke == 2)
        {
            strokeWidth = 8;
        }
        else
        {
            strokeWidth = 16;
        }
    }
    color = tool->getColor();
    for (size_t i = 0; i < 2; i++)
    {
        draggers.push_back(SkRect::MakeEmpty());
    }
}

void ShapeArrow::makePath(const int& x1, const int& y1, const int& x2, const int& y2)
{
    path.reset();
    path.moveTo(x1, y1);
    double height = 32.0;
    double width = 32.0;
    auto x = x2 - x1;
    auto y = y1 - y2;
    auto z = sqrt(x * x + y * y);
    auto sin = y / z;
    auto cos = x / z;
    // △底边的中点
    double centerX = x2 - height * cos;
    double centerY = y2 + height * sin;
    double tempA = width / 4 * sin;
    double tempB = width / 4 * cos;
    // △ 左下的顶点与底边中点之间中间位置的点
    double X1 = centerX - tempA;
    double Y1 = centerY - tempB;
    path.lineTo(X1, Y1);
    // △ 左下的顶点
    double X2 = X1 - tempA;
    double Y2 = Y1 - tempB;
    path.lineTo(X2, Y2);
    // △ 上部顶点，也就是箭头终点
    path.lineTo(x2, y2);
    // △ 右下顶点
    tempA = width / 2 * sin;
    tempB = width / 2 * cos;
    double X3 = centerX + tempA;
    double Y3 = centerY + tempB;
    path.lineTo(X3, Y3);
    // △ 右下的顶点与底边中点之间中间位置的点
    double X4 = centerX + tempA / 2;
    double Y4 = centerY + tempB / 2;
    path.lineTo(X4, Y4);
    path.lineTo(x1, y1);
}