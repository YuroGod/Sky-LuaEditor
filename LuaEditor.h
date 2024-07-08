#ifndef CANVAS_LUAEDITOR_H
#define CANVAS_LUAEDITOR_H
#include "Singleton.h"
#include "vendor/ImGuiColorTextEdit/TextEditor.h"

class LuaEditor : public Singleton<LuaEditor>
{
public:
    LuaEditor();


    void DrawExternal();
    void OpenEditor();
    void CloseEditor();
    void ToggleEditor();

private:
    void Compile(const std::string& code);
    void ApplyEditorChanges();

    void OnGameUpdate();

    bool m_Enabled;
    TextEditor m_Editor;
    void* m_State;
    bool m_TextChanged;
};


#endif //CANVAS_LUAEDITOR_H
