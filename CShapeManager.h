#pragma once

#include "CLine.h"
#include "CViewTransform.h"
#include "Command.h"
#include <memory>
#include <stack>
#include <string>
#include <vector>

// 图元管理器 / shape manager
class CShapeManager {
private:
    std::vector<std::shared_ptr<CLine>> m_shapes;
    std::stack<std::unique_ptr<ICadCommand>> m_undoStack;
    std::stack<std::unique_ptr<ICadCommand>> m_redoStack;

public:
    // 功能：添加图元。
    void AddShape(std::shared_ptr<CLine> shape);
    // 功能：移除图元。
    void RemoveShape(std::shared_ptr<CLine> shape);
    // 功能：清空图元与命令栈。
    void Clear();
    // 功能：获取可写图元列表。
    std::vector<std::shared_ptr<CLine>>& GetShapes();
    // 功能：获取只读图元列表。
    const std::vector<std::shared_ptr<CLine>>& GetShapes() const;

    // 功能：绘制全部图元。
    void DrawAll(CDC* pDC, const CViewTransform& transform, bool bShowPoints) const;

    // 功能：执行命令。
    void ExecuteCommand(std::unique_ptr<ICadCommand> cmd);
    // 功能：撤销。
    void Undo();
    // 功能：重做。
    void Redo();

    // 功能：保存 DXF。
    bool SaveToDXF(const std::wstring& filepath) const;
    // 功能：加载 DXF。
    bool LoadFromDXF(const std::wstring& filepath);
};
