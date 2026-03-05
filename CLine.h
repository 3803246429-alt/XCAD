#pragma once

#include "Point2D.h"
#include <afxwin.h>
#include <vector>

class CViewTransform;

// 线段/折线数据模型 / polyline model
class CLine {
private:
    std::vector<Point2D> m_points;
    bool m_bSelected;
    COLORREF m_color;
    bool m_hasFill;
    COLORREF m_fillColor;

public:
    // 功能：构造线条对象。
    CLine();

    // 功能：追加顶点。
    void AddPoint(const Point2D& pt);
    // 功能：设置选中状态。
    void SetSelected(bool sel);
    // 功能：读取选中状态。
    bool IsSelected() const;
    // 功能：设置颜色。
    void SetColor(COLORREF color);
    // 功能：读取颜色。
    COLORREF GetColor() const;
    // 功能：设置填充属性。
    void SetFill(bool hasFill, COLORREF fillColor);
    // 功能：读取填充开关。
    bool HasFill() const;
    // 功能：读取填充颜色。
    COLORREF GetFillColor() const;

    // 功能：读取点集。
    const std::vector<Point2D>& GetPoints() const;

    // 功能：整体平移线条。
    void Move(double dx, double dy);
    // 功能：绘制线条。
    void Draw(CDC* pDC, const CViewTransform& transform, bool bShowPoints) const;
    // 功能：命中测试。
    bool HitTest(const Point2D& pt, double tolerance) const;
};
