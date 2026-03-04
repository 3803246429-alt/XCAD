#include "pch.h"
#include "framework.h"
#include "CAD.h"
#include "CADDlg.h"

// 渲染模块 / rendering module

void CCADDlg::DrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) {
    if (nIDCtl == IDC_ABOUT_ICON) {
        CDC dc;
        dc.Attach(lpDrawItemStruct->hDC);

        CRect rc = lpDrawItemStruct->rcItem;
        dc.FillSolidRect(&rc, GetSysColor(COLOR_3DFACE));

        CRect circleRc = rc;
        circleRc.DeflateRect(1, 1);

        CPen pen(PS_SOLID, 1, RGB(0, 85, 170));
        CBrush brush(RGB(0, 122, 204));
        CPen* oldPen = dc.SelectObject(&pen);
        CBrush* oldBrush = dc.SelectObject(&brush);
        dc.Ellipse(&circleRc);
        dc.SelectObject(oldBrush);
        dc.SelectObject(oldPen);

        int oldBkMode = dc.SetBkMode(TRANSPARENT);
        COLORREF oldTextColor = dc.SetTextColor(RGB(255, 255, 255));

        CFont font;
        font.CreateFont(11, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Segoe UI"));
        CFont* oldFont = dc.SelectObject(&font);

        dc.DrawText(_T("?"), &circleRc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        dc.SelectObject(oldFont);
        dc.SetTextColor(oldTextColor);
        dc.SetBkMode(oldBkMode);

        if ((lpDrawItemStruct->itemState & ODS_FOCUS) != 0) {
            dc.DrawFocusRect(&rc);
        }

        dc.Detach();
        return;
    }

    UNREFERENCED_PARAMETER(lpDrawItemStruct);
}

void CCADDlg::OnPaint() {
    CPaintDC dc(this);

    CRect rect = m_transform.GetScreenRect();
    if (rect.IsRectEmpty()) return;

    CDC memDC;
    memDC.CreateCompatibleDC(&dc);
    CBitmap memBitmap;
    memBitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
    CBitmap* pOldBmp = memDC.SelectObject(&memBitmap);

    memDC.FillSolidRect(0, 0, rect.Width(), rect.Height(), RGB(33, 33, 33));

    DrawModel(&memDC);
    DrawPreview(&memDC);
    DrawSelection(&memDC);
    DrawCursor(&memDC);

    dc.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
    memDC.SelectObject(pOldBmp);
}

void CCADDlg::OnSize(UINT nType, int cx, int cy) {
    CDialogEx::OnSize(nType, cx, cy);

    if (GetDlgItem(IDC_DRAW_AREA)) {
        CRect rect;
        GetDlgItem(IDC_DRAW_AREA)->GetWindowRect(&rect);
        ScreenToClient(&rect);
        m_transform.SetScreenRect(rect);
        RefreshCanvas();
    }
}
