#include "pch.h"
#include "../CADDlg.h"
#include "../CADlgGeometryUtils.h"

void CCADDlg::DrawSelection(CDC* pDC) {
    if (!pDC) return;

    if (m_currentMode == CADMode::MODE_SELECT && m_bIsSelectingBox) {
        CRect box = cad::dlg::NormalizeRect(m_selectBoxStart, m_selectBoxEnd);
        CPen dashPen(PS_DASH, 1, RGB(255, 255, 255));
        CPen* oldPen = pDC->SelectObject(&dashPen);
        CBrush* oldBrush = static_cast<CBrush*>(pDC->SelectStockObject(NULL_BRUSH));
        int oldBkMode = pDC->SetBkMode(TRANSPARENT);
        pDC->Rectangle(&box);
        pDC->SetBkMode(oldBkMode);
        pDC->SelectObject(oldBrush);
        pDC->SelectObject(oldPen);
    }
}
