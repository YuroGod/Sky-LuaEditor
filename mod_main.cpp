
#include <windows.h>

#include "include/imgui.h"
#include "include/api.h"
#include "include/mod_main.h"
#include "include/global_variables.h"
#include "LuaEditor.h"


namespace gv = GlobalVariables;


void GetModInfo(ModInfo& info) {
    info.author = MOD_AUTHOR;
    info.name = MOD_NAME;
    info.description = MOD_DESCRIPTION;
    info.version = MOD_VERSION;
} 


void Start() {

}
    
void onEnable(){

}

void onDisable(){
    
}

void Render() {
    LuaEditor::GetInstance().DrawExternal();
}