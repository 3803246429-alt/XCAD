#include "pch.h"
#include "../CADDlg.h"

// 功能：开始框选流程，记录起始点并捕获鼠标。
bool CCADDlg::HandleSelectionToolLButtonDown(const CPoint& localPt) {
    if (!(m_currentMode == CADMode::MODE_SELECT && !m_bEraserCommandActive && !m_bDeleteNodeCommandActive && !m_bHatchCommandActive)) return false;

    m_bIsSelectingBox = true;
    m_selectBoxStart = localPt;
    m_selectBoxEnd = localPt;
    SetCapture();
    return true;
}

// 功能：更新框选终点，实现拖拽框实时预览。
bool CCADDlg::HandleSelectionToolMouseMove(const CPoint& localPt) {
    if (!(m_currentMode == CADMode::MODE_SELECT && m_bIsSelectingBox)) return false;

    m_selectBoxEnd = localPt;
    return true;
}

// 功能：结束框选流程，应用框选结果并释放鼠标捕获。
bool CCADDlg::HandleSelectionToolLButtonUp(const CPoint& localPt) {
    if (!(m_currentMode == CADMode::MODE_SELECT && m_bIsSelectingBox)) return false;

    m_selectBoxEnd = localPt;
    ApplySelectionBox();
    m_bIsSelectingBox = false;
    if (GetCapture() == this) {
        ReleaseCapture();
    }
    return true;
}
