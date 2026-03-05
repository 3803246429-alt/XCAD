#include "pch.h"
#include "../CADDlg.h"

namespace {
const COLORREF kCursorColor = RGB(255, 255, 255);
const int kCursorLineWidth = 1;
}

// 功能：绘制擦除/删点工具的圆形光标。
void CCADDlg::DrawCursor(CDC* pDC) {
    if (!pDC) return;

    if ((m_bEraserCommandActive || m_bDeleteNodeCommandActive) && m_bEraserCursorVisible) {
        CPen pen(PS_SOLID, kCursorLineWidth, kCursorColor);
        CPen* oldPen = pDC->SelectObject(&pen);
        CBrush* oldBrush = static_cast<CBrush*>(pDC->SelectStockObject(NULL_BRUSH));
        pDC->Ellipse(m_eraserCursor.x - m_eraserRadius, m_eraserCursor.y - m_eraserRadius,
            m_eraserCursor.x + m_eraserRadius, m_eraserCursor.y + m_eraserRadius);
        pDC->SelectObject(oldBrush);
        pDC->SelectObject(oldPen);
    }
}
