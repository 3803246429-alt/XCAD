#include "pch.h"
#include "../CADDlg.h"

void CCADDlg::DrawModel(CDC* pDC) {
    if (!pDC) return;

    m_shapeMgr.DrawAll(pDC, m_transform, m_bShowPoints);
}
