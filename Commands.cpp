#include "pch.h"
#include "Command.h"
#include "CShapeManager.h"
#include <utility>

CAddLineCommand::CAddLineCommand(CShapeManager* mgr, std::shared_ptr<CLine> line)
    : m_pManager(mgr), m_pLine(std::move(line)) {
}

void CAddLineCommand::Execute() { m_pManager->AddShape(m_pLine); }

void CAddLineCommand::Undo() { m_pManager->RemoveShape(m_pLine); }

CDeleteLinesCommand::CDeleteLinesCommand(CShapeManager* mgr, std::vector<std::shared_ptr<CLine>> lines)
    : m_pManager(mgr), m_lines(std::move(lines)) {
}

void CDeleteLinesCommand::Execute() {
    for (const auto& line : m_lines) {
        m_pManager->RemoveShape(line);
    }
}

void CDeleteLinesCommand::Undo() {
    for (const auto& line : m_lines) {
        m_pManager->AddShape(line);
    }
}
