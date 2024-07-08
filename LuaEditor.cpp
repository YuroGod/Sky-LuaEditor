#include <iostream>
#include "LuaEditor.h"
#include "Windows.h"
#include "include/api.h"

#pragma region types

typedef int (__fastcall *lua_debugdostring_t)(void *L, const char *code);

lua_debugdostring_t lua_debugdostring;

#pragma endregion

LuaEditor::LuaEditor() {
    m_TextChanged = false;

    auto base = (uintptr_t) GetModuleHandleA("Sky.exe");

    lua_debugdostring = (lua_debugdostring_t) (base + 0x1D26F0);
    uintptr_t cGame = *(uintptr_t *) (base + 0x24F62F0);

    uintptr_t luaState = *(uintptr_t *) (cGame + 0x20);

    if (m_State == nullptr)
        m_State = (void *) luaState;
}

void LuaEditor::DrawExternal() {
    ImGui::Begin("Lua Editor", nullptr, ImGuiWindowFlags_MenuBar);

    ImGui::SetWindowSize(ImVec2(300.0f * 1.6f, 300.0f), ImGuiCond_FirstUseEver);

    auto langDef = TextEditor::LanguageDefinition::Lua();
    langDef.mAutoIndentation = true;

    m_Editor.SetLanguageDefinition(langDef);

    auto cpos = m_Editor.GetCursorPosition();

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save & Compile", "Ctrl-S", nullptr, m_TextChanged)) {
                ApplyEditorChanges();
            }

            if (ImGui::MenuItem("Quit", "Esc")) {
                // Do nothing for now. Use Insert to hide this window.
                // CloseEditor();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            bool ro = m_Editor.IsReadOnly();
            if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
                m_Editor.SetReadOnly(ro);

            ImGui::Separator();

            if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && m_Editor.CanUndo()))
                m_Editor.Undo();
            if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && m_Editor.CanRedo()))
                m_Editor.Redo();

            ImGui::Separator();

            if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, m_Editor.HasSelection()))
                m_Editor.Copy();
            if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && m_Editor.HasSelection()))
                m_Editor.Cut();
            if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && m_Editor.HasSelection()))
                m_Editor.Delete();
            if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
                m_Editor.Paste();

            ImGui::Separator();

            if (ImGui::MenuItem("Select all", nullptr, nullptr))
                m_Editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(m_Editor.GetTotalLines(), 0));

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Dark palette"))
                m_Editor.SetPalette(TextEditor::GetDarkPalette());
            if (ImGui::MenuItem("Light palette"))
                m_Editor.SetPalette(TextEditor::GetLightPalette());
            if (ImGui::MenuItem("Retro blue palette"))
                m_Editor.SetPalette(TextEditor::GetRetroBluePalette());
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

#define TEXT_WITH_DESC(desc, text, ...) { ImGui::Text(text, __VA_ARGS__); if (ImGui::IsItemHovered()) ImGui::SetTooltip(desc); }
#define SEPARATOR() ImGui::SameLine(0.0f, 0.0f); ImGui::Text(" | "); ImGui::SameLine(0.0f, 0.0f);

    TEXT_WITH_DESC("Line count", "%6d/%-6d %6d lines", cpos.mLine + 1, cpos.mColumn + 1, m_Editor.GetTotalLines());
    SEPARATOR();
    TEXT_WITH_DESC("Is Overwrite", "%s", m_Editor.IsOverwrite() ? "Ovr" : "Ins");
    SEPARATOR();
    TEXT_WITH_DESC("Can undo", "%s", m_Editor.CanUndo() ? "*" : " ");

#undef TEXT_WITH_DESC
#undef SEPARATOR

    ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, ImVec4(0, 0, 0, 0));
    {
        m_Editor.Render("TextEditor");
        auto ctx = ImGui::GetCurrentContext();
    }
    ImGui::PopStyleColor();

    m_TextChanged |= m_Editor.IsTextChanged();

    ImGui::End();

}

void LuaEditor::Compile(const std::string &code) {
    if (m_State == nullptr)
        return;

    lua_debugdostring(m_State, code.c_str());
}

void LuaEditor::ApplyEditorChanges() {
    auto textToSave = m_Editor.GetText();
    Compile(textToSave);
}

void LuaEditor::OpenEditor() {
    m_Enabled = true;
}

void LuaEditor::CloseEditor() {
    m_Enabled = false;
}

void LuaEditor::ToggleEditor() {
    m_Enabled = !m_Enabled;
    if (m_Enabled)
        OpenEditor();
    else
        CloseEditor();
}

void LuaEditor::OnGameUpdate() {
}