#pragma once

#include "Point2D.h"
#include <afxwin.h>

// 视图坐标变换 / world-screen transform
class CViewTransform {
private:
    double m_scale;
    double m_offsetX;
    double m_offsetY;
    CRect m_screenRect;

public:
    // 功能：构造默认变换参数。
    CViewTransform();

    // 功能：设置屏幕矩形。
    void SetScreenRect(const CRect& rect);
    // 功能：获取屏幕矩形。
    CRect GetScreenRect() const;

    // 功能：执行缩放。
    void Zoom(double factor, const CPoint& screenCenter);
    // 功能：执行平移。
    void Pan(int deltaX, int deltaY);

    // 功能：世界坐标转屏幕坐标。
    CPoint WorldToScreen(const Point2D& pt) const;
    // 功能：屏幕坐标转世界坐标。
    Point2D ScreenToWorld(const CPoint& pt) const;
};
