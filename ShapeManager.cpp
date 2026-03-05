#include "pch.h"
#include "CShapeManager.h"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <string>
#include <utility>

namespace {
const COLORREF kCadColorWhite = RGB(255, 255, 255);
const COLORREF kCadColorRed = RGB(255, 0, 0);
const COLORREF kCadColorYellow = RGB(255, 255, 0);
const COLORREF kCadColorGreen = RGB(0, 255, 0);
const COLORREF kCadColorCyan = RGB(0, 255, 255);
const COLORREF kCadColorBlue = RGB(0, 0, 255);
const COLORREF kCadColorMagenta = RGB(255, 0, 255);

const int kDxfAciRed = 1;
const int kDxfAciYellow = 2;
const int kDxfAciGreen = 3;
const int kDxfAciCyan = 4;
const int kDxfAciBlue = 5;
const int kDxfAciMagenta = 6;
const int kDxfAciWhite = 7;

const int kDxfMinPolylinePoints = 2;

// 功能：去除文本两端空白，便于解析 DXF 行内容。
std::string Trim(const char* text) {
    std::string s = text ? text : "";
    while (!s.empty() && (s.back() == '\n' || s.back() == '\r' || std::isspace(static_cast<unsigned char>(s.back())))) {
        s.pop_back();
    }
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
        ++start;
    }
    return s.substr(start);
}

// 功能：把 RGB 颜色映射为 DXF ACI 颜色索引。
int ColorToDxfAci(COLORREF color) {
    if (color == kCadColorRed) return kDxfAciRed;
    if (color == kCadColorYellow) return kDxfAciYellow;
    if (color == kCadColorGreen) return kDxfAciGreen;
    if (color == kCadColorCyan) return kDxfAciCyan;
    if (color == kCadColorBlue) return kDxfAciBlue;
    if (color == kCadColorMagenta) return kDxfAciMagenta;
    return kDxfAciWhite;
}

// 功能：把 DXF ACI 颜色索引映射回 RGB。
COLORREF DxfAciToColor(int aci) {
    switch (aci) {
    case kDxfAciRed: return kCadColorRed;
    case kDxfAciYellow: return kCadColorYellow;
    case kDxfAciGreen: return kCadColorGreen;
    case kDxfAciCyan: return kCadColorCyan;
    case kDxfAciBlue: return kCadColorBlue;
    case kDxfAciMagenta: return kCadColorMagenta;
    case kDxfAciWhite:
    default:
        return kCadColorWhite;
    }
}
}

// 功能：向图元列表添加一个图形对象。
void CShapeManager::AddShape(std::shared_ptr<CLine> shape) {
    m_shapes.push_back(std::move(shape));
}

// 功能：从图元列表移除指定图形对象。
void CShapeManager::RemoveShape(std::shared_ptr<CLine> shape) {
    auto it = std::find(m_shapes.begin(), m_shapes.end(), shape);
    if (it != m_shapes.end()) m_shapes.erase(it);
}

// 功能：清空图形与撤销重做栈。
void CShapeManager::Clear() {
    m_shapes.clear();
    while (!m_undoStack.empty()) m_undoStack.pop();
    while (!m_redoStack.empty()) m_redoStack.pop();
}

// 功能：返回可写的图形容器引用。
std::vector<std::shared_ptr<CLine>>& CShapeManager::GetShapes() {
    return m_shapes;
}

// 功能：返回只读的图形容器引用。
const std::vector<std::shared_ptr<CLine>>& CShapeManager::GetShapes() const {
    return m_shapes;
}

// 功能：绘制所有图元。
void CShapeManager::DrawAll(CDC* pDC, const CViewTransform& transform, bool bShowPoints) const {
    for (const auto& shape : m_shapes) {
        shape->Draw(pDC, transform, bShowPoints);
    }
}

// 功能：执行命令并压入撤销栈。
void CShapeManager::ExecuteCommand(std::unique_ptr<ICadCommand> cmd) {
    cmd->Execute();
    m_undoStack.push(std::move(cmd));
    while (!m_redoStack.empty()) m_redoStack.pop();
}

// 功能：执行撤销操作。
void CShapeManager::Undo() {
    if (!m_undoStack.empty()) {
        std::unique_ptr<ICadCommand> cmd = std::move(m_undoStack.top());
        m_undoStack.pop();
        cmd->Undo();
        m_redoStack.push(std::move(cmd));
    }
}

// 功能：执行重做操作。
void CShapeManager::Redo() {
    if (!m_redoStack.empty()) {
        std::unique_ptr<ICadCommand> cmd = std::move(m_redoStack.top());
        m_redoStack.pop();
        cmd->Execute();
        m_undoStack.push(std::move(cmd));
    }
}

// 功能：将当前图元导出为 DXF 文件。
bool CShapeManager::SaveToDXF(const std::wstring& filepath) const {
    FILE* fp = nullptr;
    _wfopen_s(&fp, filepath.c_str(), L"w");
    if (!fp) return false;

    fprintf(fp, "  0\nSECTION\n  2\nENTITIES\n");
    for (const auto& shape : m_shapes) {
        const auto& pts = shape->GetPoints();
        if (pts.size() < kDxfMinPolylinePoints) continue;

        fprintf(fp, "  0\nPOLYLINE\n  8\n0\n 62\n%d\n 450\n%d\n 451\n%d\n 66\n1\n",
            ColorToDxfAci(shape->GetColor()), shape->HasFill() ? 1 : 0, ColorToDxfAci(shape->GetFillColor()));
        for (const auto& pt : pts) {
            fprintf(fp, "  0\nVERTEX\n  8\n0\n 10\n%f\n 20\n%f\n 30\n0.0\n", pt.x, pt.y);
        }
        fprintf(fp, "  0\nSEQEND\n");
    }
    fprintf(fp, "  0\nENDSEC\n  0\nEOF\n");
    fclose(fp);
    return true;
}

// 功能：从 DXF 文件加载图元数据。
bool CShapeManager::LoadFromDXF(const std::wstring& filepath) {
    FILE* fp = nullptr;
    _wfopen_s(&fp, filepath.c_str(), L"r");
    if (!fp) return false;

    Clear();

    char codeBuffer[128] = {};
    char valueBuffer[256] = {};
    std::shared_ptr<CLine> currentLine;
    bool hasPendingX = false;
    double pendingX = 0.0;

    while (fgets(codeBuffer, sizeof(codeBuffer), fp) != nullptr) {
        if (fgets(valueBuffer, sizeof(valueBuffer), fp) == nullptr) break;

        int code = std::atoi(Trim(codeBuffer).c_str());
        std::string value = Trim(valueBuffer);

        if (code == 0 && value == "POLYLINE") {
            currentLine = std::make_shared<CLine>();
            currentLine->SetColor(kCadColorWhite);
            currentLine->SetFill(false, kCadColorWhite);
            hasPendingX = false;
            continue;
        }

        if (code == 0 && value == "SEQEND") {
            if (currentLine && currentLine->GetPoints().size() >= kDxfMinPolylinePoints) {
                m_shapes.push_back(currentLine);
            }
            currentLine.reset();
            hasPendingX = false;
            continue;
        }

        if (!currentLine) continue;

        if (code == 10) {
            pendingX = std::atof(value.c_str());
            hasPendingX = true;
        } else if (code == 20 && hasPendingX) {
            double y = std::atof(value.c_str());
            currentLine->AddPoint(Point2D(pendingX, y));
            hasPendingX = false;
        } else if (code == 62) {
            currentLine->SetColor(DxfAciToColor(std::atoi(value.c_str())));
        } else if (code == 450) {
            const bool hasFill = std::atoi(value.c_str()) != 0;
            currentLine->SetFill(hasFill, currentLine->GetFillColor());
        } else if (code == 451) {
            currentLine->SetFill(currentLine->HasFill(), DxfAciToColor(std::atoi(value.c_str())));
        }
    }

    if (currentLine && currentLine->GetPoints().size() >= kDxfMinPolylinePoints) {
        m_shapes.push_back(currentLine);
    }

    fclose(fp);
    return true;
}
