/// @file
/// @author https://gist.github.com/JSandusky/54b85068aa30390c91a0b377703f042e
/// @author https://discourse.urho3d.io/t/dear-imgui-w-o-steamrolling/3960

#include "imgui.h"
#include "scriptarray/scriptarray.h"
#include "ScriptEngine.hxx"

#include <angelscript.h>
#include <string>

using namespace std;

/***ImVec2***/
static void ImVec2DefaultConstructor(ImVec2 *self) { new (self) ImVec2(); }

static void ImVec2CopyConstructor(const ImVec2 &other, ImVec2 *self) { new (self) ImVec2(other); }

static void ImVec2InitConstructor(float x, float y, ImVec2 *self) { new (self) ImVec2(x, y); }

static void ImVec2InitConstructorScaler(float s, ImVec2 *self) { new (self) ImVec2(s, s); }

// register ImVec2
void registerImVec2(asIScriptEngine *engine)
{
  int r;

  r = engine->RegisterObjectType("ImVec2", sizeof(ImVec2),
                                 asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA | asOBJ_APP_CLASS_ALLFLOATS);
  assert(r >= 0);

  // Register the object properties
  r = engine->RegisterObjectProperty("ImVec2", "float x", offsetof(ImVec2, x));
  assert(r >= 0);
  r = engine->RegisterObjectProperty("ImVec2", "float y", offsetof(ImVec2, y));
  assert(r >= 0);

  // Register the object constructors
  r = engine->RegisterObjectBehaviour("ImVec2", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ImVec2DefaultConstructor),
                                      asCALL_CDECL_OBJLAST);
  assert(r >= 0);
  r = engine->RegisterObjectBehaviour("ImVec2", asBEHAVE_CONSTRUCT, "void f(float, float)", asFUNCTION(ImVec2InitConstructor),
                                      asCALL_CDECL_OBJLAST);
  assert(r >= 0);
  r = engine->RegisterObjectBehaviour("ImVec2", asBEHAVE_CONSTRUCT, "void f(const ImVec2 &in)", asFUNCTION(ImVec2CopyConstructor),
                                      asCALL_CDECL_OBJLAST);
  assert(r >= 0);
  r = engine->RegisterObjectBehaviour("ImVec2", asBEHAVE_CONSTRUCT, "void f(float)", asFUNCTION(ImVec2InitConstructorScaler),
                                      asCALL_CDECL_OBJLAST);
  assert(r >= 0);

  // Register the object operators
  r = engine->RegisterObjectMethod("ImVec2", "float opIndex(int) const", asMETHODPR(ImVec2, operator[], (size_t) const, float),
                                   asCALL_THISCALL);
  assert(r >= 0);
  r = engine->RegisterObjectMethod("ImVec2", "ImVec2 &f(const ImVec2 &in)",
                                   asMETHODPR(ImVec2, operator=, (const ImVec2 &), ImVec2 &), asCALL_THISCALL);
  assert(r >= 0);
}

void registerImGuiBindings(asIScriptEngine *engine)
{
  engine->SetDefaultNamespace("ImGui");

  engine->RegisterGlobalFunction("bool Begin(const string&in, bool, int=0)",
                                 asFUNCTIONPR([](const string &name, bool opened, int flags)
                                              { return ImGui::Begin(name.c_str(), &opened, flags); },
                                              (const string &, bool, int), bool),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("void End()", asFUNCTIONPR(ImGui::End, (), void), asCALL_CDECL);
  engine->RegisterGlobalFunction(
      "bool BeginChild(const string&in)",
      asFUNCTIONPR([](const string &name) { return ImGui::Begin(name.c_str()); }, (const string &), bool), asCALL_CDECL);
  engine->RegisterGlobalFunction("void EndChild()", asFUNCTIONPR(ImGui::EndChild, (), void), asCALL_CDECL);

  engine->RegisterGlobalFunction("ImVec2 GetContentRegionMax()",
                                 asFUNCTIONPR(
                                     []()
                                     {
                                       auto v = ImGui::GetContentRegionMax();
                                       return ImVec2(v.x, v.y);
                                     },
                                     (), ImVec2),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("ImVec2 GetContentRegionAvail()",
                                 asFUNCTIONPR(
                                     []()
                                     {
                                       auto v = ImGui::GetContentRegionAvail();
                                       return ImVec2(v.x, v.y);
                                     },
                                     (), ImVec2),
                                 asCALL_CDECL);
  //    engine->RegisterGlobalFunction("float GetContentRegionAvailWidth()", asFUNCTIONPR(ImGui::GetContentRegionAvailWidth, (), float), asCALL_CDECL);
  engine->RegisterGlobalFunction("ImVec2 GetWindowContentRegionMin()",
                                 asFUNCTIONPR(
                                     []()
                                     {
                                       auto v = ImGui::GetWindowContentRegionMin();
                                       return ImVec2(v.x, v.y);
                                     },
                                     (), ImVec2),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("ImVec2 GetWindowContentRegionMax()",
                                 asFUNCTIONPR(
                                     []()
                                     {
                                       auto v = ImGui::GetWindowContentRegionMax();
                                       return ImVec2(v.x, v.y);
                                     },
                                     (), ImVec2),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("float GetWindowRegionWidth()", asFUNCTIONPR(ImGui::GetWindowContentRegionWidth, (), float),
                                 asCALL_CDECL);

  engine->RegisterGlobalFunction("ImVec2 GetWindowPos()",
                                 asFUNCTIONPR(
                                     []()
                                     {
                                       auto v = ImGui::GetWindowPos();
                                       return ImVec2(v.x, v.y);
                                     },
                                     (), ImVec2),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("ImVec2 GetWindowSize()",
                                 asFUNCTIONPR(
                                     []()
                                     {
                                       auto v = ImGui::GetWindowSize();
                                       return ImVec2(v.x, v.y);
                                     },
                                     (), ImVec2),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("float GetWindowWedth()", asFUNCTIONPR(ImGui::GetWindowWidth, (), float), asCALL_CDECL);
  engine->RegisterGlobalFunction("float GetWindowHeight()", asFUNCTIONPR(ImGui::GetWindowHeight, (), float), asCALL_CDECL);
  engine->RegisterGlobalFunction("bool IsWindowCollapsed()", asFUNCTIONPR(ImGui::IsWindowCollapsed, (), bool), asCALL_CDECL);
  //  engine->RegisterGlobalFunction("bool IsWindowAppearing()", asFUNCTIONPR(ImGui::IsWindowAppearing, (), bool), asCALL_CDECL);
  engine->RegisterGlobalFunction("void SetWindowFontScale(float)", asFUNCTIONPR(ImGui::SetWindowFontScale, (float), void),
                                 asCALL_CDECL);

  engine->RegisterGlobalFunction("void SetNextWindowPos(ImVec2)",
                                 asFUNCTIONPR([](ImVec2 v) { ImGui::SetNextWindowPos(ImVec2(v.x, v.y)); }, (ImVec2), void),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("void SetNextWindowSize(ImVec2)",
                                 asFUNCTIONPR([](ImVec2 v) { ImGui::SetNextWindowSize(ImVec2(v.x, v.y)); }, (ImVec2), void),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction(
      "void SetNextWindowContentSize(ImVec2)",
      asFUNCTIONPR([](ImVec2 v) { ImGui::SetNextWindowContentSize(ImVec2(v.x, v.y)); }, (ImVec2), void), asCALL_CDECL);
  engine->RegisterGlobalFunction("void SetNextWindowCollapsed(bool)",
                                 asFUNCTIONPR([](bool v) { ImGui::SetNextWindowCollapsed(v); }, (bool), void), asCALL_CDECL);
  engine->RegisterGlobalFunction("void SetNextWindowFocus()", asFUNCTIONPR([]() { ImGui::SetNextWindowFocus(); }, (), void),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("void SetWindowPos(ImVec2)",
                                 asFUNCTIONPR([](ImVec2 v) { ImGui::SetWindowPos(ImVec2(v.x, v.y)); }, (ImVec2), void),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("void SetWindowSize(ImVec2)",
                                 asFUNCTIONPR([](ImVec2 v) { ImGui::SetWindowSize(ImVec2(v.x, v.y)); }, (ImVec2), void),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("void SetWindowCollapsed(bool)",
                                 asFUNCTIONPR([](bool v) { ImGui::SetWindowCollapsed(v); }, (bool), void), asCALL_CDECL);
  engine->RegisterGlobalFunction("void SetWindowFocus()", asFUNCTIONPR([]() { ImGui::SetWindowFocus(); }, (), void),
                                 asCALL_CDECL);

  engine->RegisterGlobalFunction("void SetWindowPos(const string&in, ImVec2)",
                                 asFUNCTIONPR([](const string &name, ImVec2 v)
                                              { ImGui::SetWindowPos(name.c_str(), ImVec2(v.x, v.y)); },
                                              (const string &, ImVec2), void),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("void SetWindowSize(const string&in, ImVec2)",
                                 asFUNCTIONPR([](const string &name, ImVec2 v)
                                              { ImGui::SetWindowSize(name.c_str(), ImVec2(v.x, v.y)); },
                                              (const string &, ImVec2), void),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction(
      "void SetWindowCollapsed(const string&in, bool)",
      asFUNCTIONPR([](const string &name, bool v) { ImGui::SetWindowCollapsed(name.c_str(), v); }, (const string &, bool), void),
      asCALL_CDECL);
  engine->RegisterGlobalFunction("void SetWindowFocus(const string&in)",
                                 asFUNCTIONPR([](const string &v) { ImGui::SetWindowFocus(v.c_str()); }, (const string &), void),
                                 asCALL_CDECL);

  engine->RegisterGlobalFunction("float GetScrollX()", asFUNCTIONPR(ImGui::GetScrollX, (), float), asCALL_CDECL);
  engine->RegisterGlobalFunction("float GetScrollY()", asFUNCTIONPR(ImGui::GetScrollY, (), float), asCALL_CDECL);
  engine->RegisterGlobalFunction("float GetScrollMaxX()", asFUNCTIONPR(ImGui::GetScrollMaxX, (), float), asCALL_CDECL);
  engine->RegisterGlobalFunction("float GetScrollMaxY()", asFUNCTIONPR(ImGui::GetScrollMaxY, (), float), asCALL_CDECL);
  engine->RegisterGlobalFunction("void SetScrollX(float)", asFUNCTIONPR(ImGui::SetScrollX, (float), void), asCALL_CDECL);
  engine->RegisterGlobalFunction("void SetScrollY(float)", asFUNCTIONPR(ImGui::SetScrollY, (float), void), asCALL_CDECL);
  //    engine->RegisterGlobalFunction("void SetScrollHere(float = 0.5f)", asFUNCTIONPR(ImGui::SetScrollHere, (float), void), asCALL_CDECL);
  engine->RegisterGlobalFunction("void SetScrollFromPosY(float, float = 0.5f)",
                                 asFUNCTIONPR(ImGui::SetScrollFromPosY, (float, float), void), asCALL_CDECL);

  engine->RegisterGlobalFunction("void Separator()", asFUNCTIONPR(ImGui::Separator, (), void), asCALL_CDECL);
  engine->RegisterGlobalFunction("void SameLine(float = 0.0f, float = -1.0f)",
                                 asFUNCTIONPR(ImGui::SameLine, (float, float), void), asCALL_CDECL);
  engine->RegisterGlobalFunction("void NewLine()", asFUNCTIONPR(ImGui::NewLine, (), void), asCALL_CDECL);
  engine->RegisterGlobalFunction("void Spacing()", asFUNCTIONPR(ImGui::Spacing, (), void), asCALL_CDECL);
  engine->RegisterGlobalFunction("void Dummy(ImVec2)",
                                 asFUNCTIONPR([](ImVec2 v) { ImGui::Dummy(ImVec2(v.x, v.y)); }, (ImVec2), void), asCALL_CDECL);
  engine->RegisterGlobalFunction("void Indent(float = 0.0f)", asFUNCTIONPR(ImGui::Indent, (float), void), asCALL_CDECL);
  engine->RegisterGlobalFunction("void Unindent(float = 0.0f)", asFUNCTIONPR(ImGui::Unindent, (float), void), asCALL_CDECL);
  engine->RegisterGlobalFunction("void BeginGroup()", asFUNCTIONPR(ImGui::BeginGroup, (), void), asCALL_CDECL);
  engine->RegisterGlobalFunction("void EndGroup()", asFUNCTIONPR(ImGui::EndGroup, (), void), asCALL_CDECL);
  engine->RegisterGlobalFunction("ImVec2 GetCursorPos()",
                                 asFUNCTIONPR(
                                     []()
                                     {
                                       auto v = ImGui::GetCursorPos();
                                       return ImVec2(v.x, v.y);
                                     },
                                     (), ImVec2),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("float GetCursorPosX()", asFUNCTIONPR(ImGui::GetCursorPosX, (), float), asCALL_CDECL);
  engine->RegisterGlobalFunction("float GetCursorPosY()", asFUNCTIONPR(ImGui::GetCursorPosY, (), float), asCALL_CDECL);
  engine->RegisterGlobalFunction("void SetCursorPos(ImVec2)",
                                 asFUNCTIONPR([](ImVec2 v) { ImGui::SetCursorPos(ImVec2(v.x, v.y)); }, (ImVec2), void),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("void SetCursorPosX(float)", asFUNCTIONPR(ImGui::SetCursorPosX, (float), void), asCALL_CDECL);
  engine->RegisterGlobalFunction("void SetCursorPosY(float)", asFUNCTIONPR(ImGui::SetCursorPosY, (float), void), asCALL_CDECL);
  engine->RegisterGlobalFunction("ImVec2 GetCursorStartPos()",
                                 asFUNCTIONPR(
                                     []()
                                     {
                                       auto v = ImGui::GetCursorStartPos();
                                       return ImVec2(v.x, v.y);
                                     },
                                     (), ImVec2),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("ImVec2 GetCursorScreenPos()",
                                 asFUNCTIONPR(
                                     []()
                                     {
                                       auto v = ImGui::GetCursorScreenPos();
                                       return ImVec2(v.x, v.y);
                                     },
                                     (), ImVec2),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("void SetCursorScreenPos(ImVec2)",
                                 asFUNCTIONPR([](ImVec2 v) { ImGui::SetCursorScreenPos(ImVec2(v.x, v.y)); }, (ImVec2), void),
                                 asCALL_CDECL);
  //  engine->RegisterGlobalFunction("void AlignTextToFramePadding()", asFUNCTIONPR(ImGui::AlignTextToFramePadding, (), void), asCALL_CDECL);
  engine->RegisterGlobalFunction("float GetTextLineHeight()", asFUNCTIONPR(ImGui::GetTextLineHeight, (), float), asCALL_CDECL);
  engine->RegisterGlobalFunction("float GetTextLineHeightWithSpacing()",
                                 asFUNCTIONPR(ImGui::GetTextLineHeightWithSpacing, (), float), asCALL_CDECL);
  //  engine->RegisterGlobalFunction("float GetFrameHeight()", asFUNCTIONPR(ImGui::GetFrameHeight, (), float), asCALL_CDECL);
  //  engine->RegisterGlobalFunction("float GetFrameHeightWithSpacing()", asFUNCTIONPR(ImGui::GetFrameHeightWithSpacing, (), float), asCALL_CDECL);

  // Columns
  engine->RegisterGlobalFunction("void Columns(int = 1, const string&in = string(), bool = true)",
                                 asFUNCTIONPR([](int a, const string &b, bool c)
                                              { ImGui::Columns(a, b.empty() ? b.c_str() : 0x0, c); },
                                              (int, const string &, bool), void),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("void NextColumn()", asFUNCTIONPR([]() { ImGui::NextColumn(); }, (), void), asCALL_CDECL);
  engine->RegisterGlobalFunction("int GetColumnIndex()", asFUNCTIONPR([]() { return ImGui::GetColumnIndex(); }, (), int),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("float GetColumnWidth(int = -1)",
                                 asFUNCTIONPR([](int a) { return ImGui::GetColumnWidth(a); }, (int), float), asCALL_CDECL);
  //  engine->RegisterGlobalFunction("void SetColumnWidth(int, float)", asFUNCTIONPR([](int a, float b) {  ImGui::SetColumnWidth(a, b);  }, (int,float), void), asCALL_CDECL);
  engine->RegisterGlobalFunction("float GetColumnOffset(int = -1)",
                                 asFUNCTIONPR([](int a) { return ImGui::GetColumnOffset(a); }, (int), float), asCALL_CDECL);
  engine->RegisterGlobalFunction("void SetColumnOffset(int, float)",
                                 asFUNCTIONPR([](int a, float b) { ImGui::SetColumnOffset(a, b); }, (int, float), void),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("int GetColumnsCount()", asFUNCTIONPR([]() { return ImGui::GetColumnsCount(); }, (), int),
                                 asCALL_CDECL);

  // ID scopes
  engine->RegisterGlobalFunction("void PushID(const string&in)",
                                 asFUNCTIONPR([](const string &n) { ImGui::PushID(n.c_str()); }, (const string &), void),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("void PushID(int int_id)", asFUNCTIONPR([](int id) { ImGui::PushID(id); }, (int), void),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("void PopID()", asFUNCTIONPR(ImGui::PopID, (), void), asCALL_CDECL);
  engine->RegisterGlobalFunction(
      "uint GetID(const string&in)",
      asFUNCTIONPR([](const string &n) { return ImGui::GetID(n.c_str()); }, (const string &), unsigned), asCALL_CDECL);

  // Widgets: Text
  engine->RegisterGlobalFunction("void Text(const string&in)",
                                 asFUNCTIONPR([](const string &n) { ImGui::Text(n.c_str()); }, (const string &), void),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("void TextDisabled(const string&in)",
                                 asFUNCTIONPR([](const string &n) { ImGui::TextDisabled(n.c_str()); }, (const string &), void),
                                 asCALL_CDECL);
  //  engine->RegisterGlobalFunction("void TextColored(color col, const string&in)",
  //                                 asFUNCTIONPR([](ColourValue c, const string &n)
  //                                              { ImGui::TextColored(ImVec4(c.r, c.g, c.b, c.a), n.c_str()); },
  //                                              (ColourValue, const string &), void),
  //                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("void TextWrapped(const string&in)",
                                 asFUNCTIONPR([](const string &n) { ImGui::TextWrapped(n.c_str()); }, (const string &), void),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("void LabelText(const string&in, const string&in)",
                                 asFUNCTIONPR([](const string &l, const string &n) { ImGui::LabelText(l.c_str(), n.c_str()); },
                                              (const string &, const string &), void),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("void Bullet()", asFUNCTIONPR(ImGui::Bullet, (), void), asCALL_CDECL);
  engine->RegisterGlobalFunction("void BulletText(const string&in)",
                                 asFUNCTIONPR([](const string &n) { ImGui::BulletText(n.c_str()); }, (const string &), void),
                                 asCALL_CDECL);

  // Widgets: Main
  engine->RegisterGlobalFunction("bool Button(const string&in, ImVec2 = ImVec2(0,0))",
                                 asFUNCTIONPR([](const string &n, ImVec2 v)
                                              { return ImGui::Button(n.c_str(), ImVec2(v.x, v.y)); },
                                              (const string &, ImVec2), bool),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction(
      "bool SmallButton(const string&in)",
      asFUNCTIONPR([](const string &n) { return ImGui::SmallButton(n.c_str()); }, (const string &), bool), asCALL_CDECL);
  engine->RegisterGlobalFunction("bool InvisibleButton(const string&in, ImVec2)",
                                 asFUNCTIONPR([](const string &id, ImVec2 v)
                                              { return ImGui::InvisibleButton(id.c_str(), ImVec2(v.x, v.y)); },
                                              (const string &, ImVec2), bool),
                                 asCALL_CDECL);
  //  engine->RegisterGlobalFunction("void Image(const TexturePtr&in, ImVec2)",
  //                                 asFUNCTIONPR([](TexturePtr const &tex, ImVec2 v)
  //                                              { ImGui::Image((ImTextureID)tex->getHandle(), ImVec2(v.x, v.y)); },
  //                                              (TexturePtr const &, ImVec2), void),
  //                                 asCALL_CDECL);
  engine->RegisterGlobalFunction(
      "bool Checkbox(const string&in, bool&inout)",
      asFUNCTIONPR([](const string &n, bool &v) { return ImGui::Checkbox(n.c_str(), &v); }, (const string &, bool &), bool),
      asCALL_CDECL);
  engine->RegisterGlobalFunction("bool CheckboxFlags(const string&in, uint&inout, uint)",
                                 asFUNCTIONPR([](const string &n, unsigned &f, unsigned v)
                                              { return ImGui::CheckboxFlags(n.c_str(), &f, v); },
                                              (const string &, unsigned &, unsigned), bool),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction(
      "bool RadioButton(const string&in, bool)",
      asFUNCTIONPR([](const string &n, bool v) { return ImGui::RadioButton(n.c_str(), v); }, (const string &, bool), bool),
      asCALL_CDECL);
  engine->RegisterGlobalFunction("bool RadioButton(const string&in, int&inout, int)",
                                 asFUNCTIONPR([](const string &n, int &v, int vv)
                                              { return ImGui::RadioButton(n.c_str(), &v, vv); },
                                              (const string &, int &, int), bool),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("void ProgressBar(float)", asFUNCTIONPR([](float v) { ImGui::ProgressBar(v); }, (float), void),
                                 asCALL_CDECL);

  // Widgets: Combo Box
  //   engine->RegisterGlobalFunction("bool BeginCombo(const string&in, const string&in, int = 0)", asFUNCTIONPR([](const string& id, const string& prevItem, int flags) { return ImGui::BeginCombo(id.c_str(), prevItem.c_str(), flags); }, (const string&, const string&, int), bool), asCALL_CDECL);
  //   engine->RegisterGlobalFunction("void EndCombo()", asFUNCTIONPR(ImGui::EndCombo, (), void), asCALL_CDECL);
  /*
    static char imgui_comboItem[4096];
    engine->RegisterGlobalFunction("bool Combo(const string&in, int&inout, const Array<string>@+)", asFUNCTIONPR([](const string& lbl, int& index, const CScriptArray* items) {
        memset(imgui_comboItem, 0, sizeof(char) * 4096);
        unsigned offset = 0;
        for (unsigned i = 0; i < items->GetSize(); ++i)
        {
            string* str = ((string*)items->At(i));
            strcpy(imgui_comboItem + offset, str->c_str());
            offset += str->length() + 1;
        }
        return ImGui::Combo(lbl.c_str(), &index, imgui_comboItem, -1);
    }, (const string&, int&, const CScriptArray*), bool), asCALL_CDECL);
    */
  // Widgets: Drags
  engine->RegisterGlobalFunction("bool DragFloat(const string&in, float&inout, float = 1.0f, float = 0.0f, float = 0.0f)",
                                 asFUNCTIONPR([](const string &n, float &v, float speed, float mn, float mx)
                                              { return ImGui::DragFloat(n.c_str(), &v, speed, mn, mx); },
                                              (const string &, float &, float, float, float), bool),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("bool DragFloat2(const string&in, ImVec2&inout)",
                                 asFUNCTIONPR([](const string &n, ImVec2 &v) { return ImGui::DragFloat2(n.c_str(), &v.x); },
                                              (const string &, ImVec2 &), bool),
                                 asCALL_CDECL);
  //    engine->RegisterGlobalFunction("bool DragFloat3(const string&in, vector3&inout)", asFUNCTIONPR([](const string& n, Vector3& v) {
  //        return ImGui::DragFloat3(n.c_str(), &v.x); }, (const string&, Vector3&), bool), asCALL_CDECL);
  /* --- TODO: Register Vector4
    engine->RegisterGlobalFunction("bool DragFloat4(const string&in, Vector4&inout)", asFUNCTIONPR([](const string& n, Vector4& v) {
        return ImGui::DragFloat4(n.c_str(), &v.x); }, (const string&, Vector4&), bool), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool DragInt(const string&in, int&inout, int = 0, int = 0)", asFUNCTIONPR([](const string& n, int& v, int mn, int mx) {
        return ImGui::DragInt(n.c_str(), &v, 1.0f, mn, mx); }, (const string&, int&, int, int), bool), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool DragInt2(const string&in, IntImVec2&inout, int = 0, int = 0)", asFUNCTIONPR([](const string& n, IntImVec2& v, int mn, int mx) {
        return ImGui::DragInt2(n.c_str(), &v.x, 1.0f, mn, mx); }, (const string&, IntImVec2&, int,int), bool), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool DragInt3(const string&in, IntVector3&inout, int = 0, int = 0)", asFUNCTIONPR([](const string& n, IntVector3& v, int mn, int mx) {
        return ImGui::DragInt3(n.c_str(), &v.x, 1.0f, mn, mx); }, (const string&, IntVector3&, int,int), bool), asCALL_CDECL);
        */
  engine->RegisterGlobalFunction("bool DragFloatRange2(const string&in, float&inout, float&inout, float = 0.0f, float = 1.0f)",
                                 asFUNCTIONPR([](const string &n, float &v0, float &v1, float mn, float mx)
                                              { return ImGui::DragFloatRange2(n.c_str(), &v0, &v1, 1.0f, mn, mx); },
                                              (const string &, float &, float &, float, float), bool),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("bool DragIntRange2(const string&in, int&inout, int&inout, int, int)",
                                 asFUNCTIONPR([](const string &n, int &v0, int &v1, int mn, int mx)
                                              { return ImGui::DragIntRange2(n.c_str(), &v0, &v1, 1.0f, mn, mx); },
                                              (const string &, int &, int &, int, int), bool),
                                 asCALL_CDECL);

  // Widgets: Input with Keyboard
  static char imgui_text_buffer[4096]; // shared with multiple widgets
  engine->RegisterGlobalFunction("bool InputText(const string&in, string&inout)",
                                 asFUNCTIONPR(
                                     [](const string &id, string &val)
                                     {
                                       memset(imgui_text_buffer, 0, sizeof(char) * 4096);
                                       strcpy(imgui_text_buffer, val.c_str());
                                       if (ImGui::InputText(id.c_str(), imgui_text_buffer, 4096))
                                       {
                                         val = imgui_text_buffer;
                                         return true;
                                       }
                                       return false;
                                     },
                                     (const string &, string &), bool),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("bool InputTextMultiline(const string&in, string&inout)",
                                 asFUNCTIONPR(
                                     [](const string &id, string &val)
                                     {
                                       memset(imgui_text_buffer, 0, sizeof(char) * 4096);
                                       strcpy(imgui_text_buffer, val.c_str());
                                       if (ImGui::InputTextMultiline(id.c_str(), imgui_text_buffer, 4096))
                                       {
                                         val = imgui_text_buffer;
                                         return true;
                                       }
                                       return false;
                                     },
                                     (const string &, string &), bool),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("bool InputFloat(const string&, float&inout)",
                                 asFUNCTIONPR([](const string &id, float &val) { return ImGui::InputFloat(id.c_str(), &val); },
                                              (const string &, float &), bool),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("bool InputFloat2(const string&, ImVec2&inout)",
                                 asFUNCTIONPR([](const string &id, ImVec2 &val)
                                              { return ImGui::InputFloat2(id.c_str(), &val.x); },
                                              (const string &, ImVec2 &), bool),
                                 asCALL_CDECL);
  //  engine->RegisterGlobalFunction("bool InputFloat3(const string&, vector3&inout)",
  //                                 asFUNCTIONPR([](const string &id, Vector3 &val)
  //                                              { return ImGui::InputFloat3(id.c_str(), &val.x); },
  //                                              (const string &, Vector3 &), bool),
  //                                 asCALL_CDECL);
  /* --- TODO: Register Vector4
    engine->RegisterGlobalFunction("bool InputFloat4(const string&, Vector4&inout)", asFUNCTIONPR([](const string& id, Vector4& val) {
        return ImGui::InputFloat4(id.c_str(), &val.x_); }, (const string&, Vector4&),bool), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool InputInt(const string&, int&inout)", asFUNCTIONPR([](const string& id, int& val) {
        return ImGui::InputInt(id.c_str(), &val); }, (const string&, int&), bool), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool InputInt2(const string&, IntImVec2&inout)", asFUNCTIONPR([](const string& id, IntImVec2& val) {
        return ImGui::InputInt2(id.c_str(), &val.x_); }, (const string&, IntImVec2&),bool), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool InputInt3(const string&, IntVector3&inout)", asFUNCTIONPR([](const string& id, IntVector3& val) {
        return ImGui::InputInt3(id.c_str(), &val.x_); }, (const string&, IntVector3&), bool), asCALL_CDECL);
        */

  // Widgets: Sliders (tip: ctrl+click on a slider to input with keyboard. manually input values aren't clamped, can go off-bounds)
  engine->RegisterGlobalFunction("bool SliderFloat(const string&in, float&inout, float = 0.0f, float = 0.0f)",
                                 asFUNCTIONPR([](const string &n, float &v, float mn, float mx)
                                              { return ImGui::SliderFloat(n.c_str(), &v, mn, mx); },
                                              (const string &, float &, float, float), bool),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("bool SliderFloat2(const string&in, ImVec2&inout, float, float)",
                                 asFUNCTIONPR([](const string &n, ImVec2 &v, float mn, float mx)
                                              { return ImGui::SliderFloat2(n.c_str(), &v.x, mn, mx); },
                                              (const string &, ImVec2 &, float, float), bool),
                                 asCALL_CDECL);
  //  engine->RegisterGlobalFunction("bool SliderFloat3(const string&in, vector3&inout, float, float)",
  //                                 asFUNCTIONPR([](const string &n, Vector3 &v, float mn, float mx)
  //                                              { return ImGui::SliderFloat3(n.c_str(), &v.x, mn, mx); },
  //                                              (const string &, Vector3 &, float, float), bool),
  //                                 asCALL_CDECL);
  /* --- TODO: Register Vector4
    engine->RegisterGlobalFunction("bool SliderFloat4(const string&in, Vector4&inout, float, float)", asFUNCTIONPR([](const string& n, Vector4& v, float mn, float mx) {
        return ImGui::SliderFloat4(n.c_str(), &v.x, mn, mx); }, (const string&, Vector4&,float,float),bool), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool SliderInt(const string&in, int&inout, int = 0, int = 0)", asFUNCTIONPR([](const string& n, int& v, int mn, int mx) {
        return ImGui::SliderInt(n.c_str(), &v, mn, mx); }, (const string&, int&,int,int), bool), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool SliderInt2(const string&in, IntImVec2&inout, int = 0, int = 0)", asFUNCTIONPR([](const string& n, IntImVec2& v, int mn, int mx) {
        return ImGui::SliderInt2(n.c_str(), &v.x, mn, mx); }, (const string&, IntImVec2&, int,int),bool), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool SliderInt3(const string&in, IntVector3&inout, int = 0, int = 0)", asFUNCTIONPR([](const string& n, IntVector3& v, int mn, int mx) {
        return ImGui::SliderInt3(n.c_str(), &v.x, mn, mx); }, (const string&, IntVector3&, int,int),bool), asCALL_CDECL);
        */

  // Widgets: Color Editor/Picker
  //  engine->RegisterGlobalFunction("bool ColorEdit3(const string&in, color&inout)",
  //                                 asFUNCTIONPR(
  //                                     [](const string &id, ColourValue &val)
  //                                     {
  //                                       Vector3 v(val.r, val.g, val.b);
  //                                       if (ImGui::ColorEdit3(id.c_str(), &v.x))
  //                                       {
  //                                         val = ColourValue(v.x, v.y, v.z);
  //                                         return true;
  //                                       }
  //                                       return false;
  //                                     },
  //                                     (const string &, ColourValue &), bool),
  //                                 asCALL_CDECL);
  //  engine->RegisterGlobalFunction("bool ColorEdit4(const string&in, color&inout)",
  //                                 asFUNCTIONPR(
  //                                     [](const string &id, ColourValue &val)
  //                                     {
  //                                       Vector4 v(val.r, val.g, val.b, val.a);
  //                                       if (ImGui::ColorEdit4(id.c_str(), &v.x))
  //                                       {
  //                                         val = ColourValue(v.x, v.y, v.z, v.w);
  //                                         return true;
  //                                       }
  //                                       return false;
  //                                     },
  //                                     (const string &, ColourValue &), bool),
  //                                 asCALL_CDECL);
  /*   engine->RegisterGlobalFunction("bool ColorPicker3(const string&in, color&inout)", asFUNCTIONPR([](const string& id, ColourValue& val) {
        Vector3 v(val.r, val.g, val.b);
        if (ImGui::ColorPicker3(id.c_str(), &v.x))
        {
            val = ColourValue(v.x, v.y, v.z);
            return true;
        }
        return false;
    }, (const string&, ColourValue&),bool), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool ColorPicker4(const string&in, color&inout)", asFUNCTIONPR([](const string& id, ColourValue& val) {
        Vector4 v(val.r, val.g, val.b, val.a);
        if (ImGui::ColorPicker4(id.c_str(), &v.x))
        {
            val = ColourValue(v.x, v.y, v.z, v.w);
            return true;
        }
        return false;
    }, (const string&, ColourValue&),bool), asCALL_CDECL);*/
  //  engine->RegisterGlobalFunction("bool ColorButton(const string&in, color)",
  //                                 asFUNCTIONPR(
  //                                     [](const string &id, ColourValue val)
  //                                     {
  //                                       Vector4 v(val.r, val.g, val.b, val.a);
  //                                       ImVec4 vv(v.x, v.y, v.z, v.w);
  //                                       return ImGui::ColorButton(id.c_str(), vv);
  //                                     },
  //                                     (const string &, ColourValue), bool),
  //                                 asCALL_CDECL);

  // Widgets: Trees
  engine->RegisterGlobalFunction(
      "bool TreeNode(const string&in)",
      asFUNCTIONPR([](const string &id) { return ImGui::TreeNode(id.c_str()); }, (const string &), bool), asCALL_CDECL);
  engine->RegisterGlobalFunction("void TreePush(const string&in)",
                                 asFUNCTIONPR([](const string &id) { ImGui::TreePush(id.c_str()); }, (const string &), void),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("void TreePop()", asFUNCTIONPR(ImGui::TreePop, (), void), asCALL_CDECL);
  //  engine->RegisterGlobalFunction("void TreeAdvanceToLabelPos()", asFUNCTIONPR(ImGui::TreeAdvanceToLabelPos, (), void),
  //                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("float GetTreeNodeToLabelSpacing()", asFUNCTIONPR(ImGui::GetTreeNodeToLabelSpacing, (), float),
                                 asCALL_CDECL);
  //  engine->RegisterGlobalFunction("void SetNextTreeNodeOpen(bool)",
  //                                 asFUNCTIONPR([](bool val) { ImGui::SetNextTreeNodeOpen(val); }, (bool), void), asCALL_CDECL);
  engine->RegisterGlobalFunction(
      "bool CollapsingHeader(const string&in)",
      asFUNCTIONPR([](const string &n) { return ImGui::CollapsingHeader(n.c_str()); }, (const string &), bool), asCALL_CDECL);
  engine->RegisterGlobalFunction("bool CollapsingHeader(const string&in, bool&inout)",
                                 asFUNCTIONPR([](const string &n, bool &v) { return ImGui::CollapsingHeader(n.c_str(), &v); },
                                              (const string &, bool &), bool),
                                 asCALL_CDECL);

  // Widgets: Selectable / Lists
  engine->RegisterGlobalFunction(
      "bool Selectable(const string&in, bool = false)",
      asFUNCTIONPR([](const string &n, bool v) { return ImGui::Selectable(n.c_str(), v); }, (const string &, bool), bool),
      asCALL_CDECL);
  engine->RegisterGlobalFunction(
      "bool ListBoxHeader(const string&in)",
      asFUNCTIONPR([](const string &n) { return ImGui::ListBoxHeader(n.c_str()); }, (const string &), bool), asCALL_CDECL);

  // Values
  engine->RegisterGlobalFunction(
      "void Value(const string&in, bool)",
      asFUNCTIONPR([](const string &n, bool v) { ImGui::Value(n.c_str(), v); }, (const string &, bool), void), asCALL_CDECL);
  engine->RegisterGlobalFunction(
      "void Value(const string&in, int)",
      asFUNCTIONPR([](const string &n, int v) { ImGui::Value(n.c_str(), v); }, (const string &, int), void), asCALL_CDECL);
  engine->RegisterGlobalFunction(
      "void Value(const string&in, uint)",
      asFUNCTIONPR([](const string &n, unsigned v) { ImGui::Value(n.c_str(), v); }, (const string &, unsigned), void),
      asCALL_CDECL);
  engine->RegisterGlobalFunction(
      "void Value(const string&in, float)",
      asFUNCTIONPR([](const string &n, float v) { ImGui::Value(n.c_str(), v); }, (const string &, float), void), asCALL_CDECL);

  // Tooltips
  engine->RegisterGlobalFunction("void BeginTooltip()", asFUNCTIONPR(ImGui::BeginTooltip, (), void), asCALL_CDECL);
  engine->RegisterGlobalFunction("void EndTooltip()", asFUNCTIONPR(ImGui::EndTooltip, (), void), asCALL_CDECL);
  engine->RegisterGlobalFunction("void SetTooltip(const string&in)",
                                 asFUNCTIONPR([](const string &t) { ImGui::SetTooltip(t.c_str()); }, (const string &), void),
                                 asCALL_CDECL);

  // Menus
  engine->RegisterGlobalFunction("bool BeginMainMenuBar()", asFUNCTIONPR([]() { return ImGui::BeginMainMenuBar(); }, (), bool),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("void EndMainMenuBar()", asFUNCTIONPR([]() { ImGui::EndMainMenuBar(); }, (), void),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("bool BeginMenuBar()", asFUNCTIONPR([]() { return ImGui::BeginMenuBar(); }, (), bool),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("void EndMenuBar()", asFUNCTIONPR([]() { ImGui::EndMenuBar(); }, (), void), asCALL_CDECL);
  engine->RegisterGlobalFunction("bool BeginMenu(const string&in, bool = true)",
                                 asFUNCTIONPR([](const string &a, bool b)
                                              { return ImGui::BeginMenu(a.empty() ? a.c_str() : 0x0, b); },
                                              (const string &, bool), bool),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("void EndMenu()", asFUNCTIONPR([]() { ImGui::EndMenu(); }, (), void), asCALL_CDECL);
  engine->RegisterGlobalFunction(
      "bool MenuItem(const string&in, const string&in = string(), bool = false, bool = true)",
      asFUNCTIONPR([](const string &a, const string &b, bool c, bool d)
                   { return ImGui::MenuItem(a.empty() ? a.c_str() : 0x0, b.empty() ? b.c_str() : 0x0, c, d); },
                   (const string &, const string &, bool, bool), bool),
      asCALL_CDECL);
  engine->RegisterGlobalFunction(
      "bool MenuItem(const string&in, const string&in, bool &inout, bool = true)",
      asFUNCTIONPR([](const string &a, const string &b, bool &c, bool d)
                   { return ImGui::MenuItem(a.empty() ? a.c_str() : 0x0, b.empty() ? b.c_str() : 0x0, &c, d); },
                   (const string &, const string &, bool &, bool), bool),
      asCALL_CDECL);

  // Popups
  engine->RegisterGlobalFunction(
      "void OpenPopup(const string&in)",
      asFUNCTIONPR([](const string &a) { ImGui::OpenPopup(a.empty() ? a.c_str() : 0x0); }, (const string &), void), asCALL_CDECL);
  /* engine->RegisterGlobalFunction("bool BeginPopup(const string&in, int = 0)", asFUNCTIONPR([](const string& a, int b) {
        return ImGui::BeginPopup(a.empty() ? a.c_str() : 0x0, (ImGuiWindowFlags)b);  }, (const string&, int), bool), asCALL_CDECL);*/ // FIXME: update imgui!
  engine->RegisterGlobalFunction("bool BeginPopup(const string&in, int = 0)",
                                 asFUNCTIONPR([](const string &a, int b)
                                              { return ImGui::BeginPopup(a.empty() ? a.c_str() : 0x0); },
                                              (const string &, int), bool),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("bool BeginPopupContextItem(const string&in = string(), int = 1)",
                                 asFUNCTIONPR([](const string &a, int b)
                                              { return ImGui::BeginPopupContextItem(a.empty() ? a.c_str() : 0x0, b); },
                                              (const string &, int), bool),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("bool BeginPopupContextWindow(const string&in = string(), int = 1, bool = true)",
                                 asFUNCTIONPR([](const string &a, int b, bool c)
                                              { return ImGui::BeginPopupContextWindow(a.empty() ? a.c_str() : 0x0, b, c); },
                                              (const string &, int, bool), bool),
                                 asCALL_CDECL); // FIXME: update imgui! -- swapped args
  engine->RegisterGlobalFunction("bool BeginPopupContextVoid(const string&in = string(), int = 1)",
                                 asFUNCTIONPR([](const string &a, int b)
                                              { return ImGui::BeginPopupContextVoid(a.empty() ? a.c_str() : 0x0, b); },
                                              (const string &, int), bool),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction(
      "bool BeginPopupModal(const string&in, bool &inout = null, int = 0)",
      asFUNCTIONPR([](const string &a, bool &b, int c)
                   { return ImGui::BeginPopupModal(a.empty() ? a.c_str() : 0x0, &b, (ImGuiWindowFlags)c); },
                   (const string &, bool &, int), bool),
      asCALL_CDECL);
  engine->RegisterGlobalFunction("void EndPopup()", asFUNCTIONPR([]() { ImGui::EndPopup(); }, (), void), asCALL_CDECL);
  /*    engine->RegisterGlobalFunction("bool OpenPopupOnItemClick(const string&in = string(), int = 1)", asFUNCTIONPR([](const string& a, int b) {
        return ImGui::OpenPopupOnItemClick(a.empty() ? a.c_str() : 0x0, b);  }, (const string&, int), bool), asCALL_CDECL);*/ // FIXME: update imgui!
  /*   engine->RegisterGlobalFunction("bool IsPopupOpen(const string&in)", asFUNCTIONPR([](const string& a) {
        return ImGui::IsPopupOpen(a.empty() ? a.c_str() : 0x0);  }, (const string&), bool), asCALL_CDECL); */ // FIXME: update imgui!
  engine->RegisterGlobalFunction("void CloseCurrentPopup()", asFUNCTIONPR([]() { ImGui::CloseCurrentPopup(); }, (), void),
                                 asCALL_CDECL);

  // Clip-rects
  engine->RegisterGlobalFunction("void PushClipRect(const ImVec2&, const ImVec2&, bool)",
                                 asFUNCTIONPR([](const ImVec2 &a, const ImVec2 &b, bool c)
                                              { ImGui::PushClipRect(ImVec2(a.x, a.y), ImVec2(b.x, b.y), c); },
                                              (const ImVec2 &, const ImVec2 &, bool), void),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("void PopClipRect()", asFUNCTIONPR([]() { ImGui::PopClipRect(); }, (), void), asCALL_CDECL);

  // Focus
  /*   engine->RegisterGlobalFunction("void SetItemDefaultFocus()", asFUNCTIONPR([]() {  ImGui::SetItemDefaultFocus();  }, (), void), asCALL_CDECL); */ // TODO update imgui
  engine->RegisterGlobalFunction("void SetKeyboardFocusHere(int = 0)",
                                 asFUNCTIONPR([](int a) { ImGui::SetKeyboardFocusHere(a); }, (int), void), asCALL_CDECL);

  // Utilities
  engine->RegisterGlobalFunction("bool IsItemHovered(int = 0)",
                                 asFUNCTIONPR([](int a) { return ImGui::IsItemHovered(); }, (int), bool),
                                 asCALL_CDECL); // TODO: update imgui -- flags omitted
  engine->RegisterGlobalFunction("bool IsItemActive()", asFUNCTIONPR([]() { return ImGui::IsItemActive(); }, (), bool),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("bool IsItemClicked(int = 0)",
                                 asFUNCTIONPR([](int a) { return ImGui::IsItemClicked(a); }, (int), bool), asCALL_CDECL);
  engine->RegisterGlobalFunction("bool IsItemVisible()", asFUNCTIONPR([]() { return ImGui::IsItemVisible(); }, (), bool),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("bool IsAnyItemHovered()", asFUNCTIONPR([]() { return ImGui::IsAnyItemHovered(); }, (), bool),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("bool IsAnyItemActive()", asFUNCTIONPR([]() { return ImGui::IsAnyItemActive(); }, (), bool),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("ImVec2 GetItemRectMin()",
                                 asFUNCTIONPR(
                                     []()
                                     {
                                       auto v = ImGui::GetItemRectMin();
                                       return ImVec2(v.x, v.y);
                                     },
                                     (), ImVec2),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("ImVec2 GetItemRectMax()",
                                 asFUNCTIONPR(
                                     []()
                                     {
                                       auto v = ImGui::GetItemRectMax();
                                       return ImVec2(v.x, v.y);
                                     },
                                     (), ImVec2),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("ImVec2 GetItemRectSize()",
                                 asFUNCTIONPR(
                                     []()
                                     {
                                       auto v = ImGui::GetItemRectSize();
                                       return ImVec2(v.x, v.y);
                                     },
                                     (), ImVec2),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("void SetItemAllowOverlap()", asFUNCTIONPR([]() { ImGui::SetItemAllowOverlap(); }, (), void),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("bool IsWindowFocused(int = 0)",
                                 asFUNCTIONPR([](int a) { return ImGui::IsWindowFocused(); }, (int), bool),
                                 asCALL_CDECL); // TODO: update imgui -- flags omitted
  engine->RegisterGlobalFunction("bool IsWindowHovered(int = 0)",
                                 asFUNCTIONPR([](int a) { return ImGui::IsWindowHovered(); }, (int), bool),
                                 asCALL_CDECL); // TODO: update imgui -- flags omitted
  engine->RegisterGlobalFunction(
      "bool IsRectVisible(const ImVec2&)",
      asFUNCTIONPR([](const ImVec2 &a) { return ImGui::IsRectVisible(ImVec2(a.x, a.y)); }, (const ImVec2 &), bool), asCALL_CDECL);
  engine->RegisterGlobalFunction("bool IsRectVisible(const ImVec2&, const ImVec2&)",
                                 asFUNCTIONPR([](const ImVec2 &a, const ImVec2 &b)
                                              { return ImGui::IsRectVisible(ImVec2(a.x, a.y), ImVec2(b.x, b.y)); },
                                              (const ImVec2 &, const ImVec2 &), bool),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("float GetTime()", asFUNCTIONPR([]() { return (float)ImGui::GetTime(); }, (), float),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("int GetFrameCount()", asFUNCTIONPR([]() { return ImGui::GetFrameCount(); }, (), int),
                                 asCALL_CDECL);

  engine->RegisterGlobalFunction("ImVec2 CalcTextSize(const string&in, const string&in = string(), bool = false, float = -1.0f)",
                                 asFUNCTIONPR(
                                     [](const string &a, const string &b, bool c, float d)
                                     {
                                       auto v =
                                           ImGui::CalcTextSize(a.empty() ? a.c_str() : 0x0, b.empty() ? b.c_str() : 0x0, c, d);
                                       return ImVec2(v.x, v.y);
                                     },
                                     (const string &, const string &, bool, float), ImVec2),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("void CalcListClipping(int, float, int&inout, int&inout)",
                                 asFUNCTIONPR([](int a, float b, int &c, int &d) { ImGui::CalcListClipping(a, b, &c, &d); },
                                              (int, float, int &, int &), void),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("bool BeginChildFrame(uint, const ImVec2&, int = 0)",
                                 asFUNCTIONPR([](unsigned a, const ImVec2 &b, int c)
                                              { return ImGui::BeginChildFrame(a, ImVec2(b.x, b.y), (ImGuiWindowFlags)c); },
                                              (unsigned, const ImVec2 &, int), bool),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("void EndChildFrame()", asFUNCTIONPR([]() { ImGui::EndChildFrame(); }, (), void), asCALL_CDECL);

  engine->RegisterGlobalFunction("int GetKeyIndex(int)",
                                 asFUNCTIONPR([](int a) { return ImGui::GetKeyIndex((ImGuiKey)a); }, (int), int), asCALL_CDECL);
  engine->RegisterGlobalFunction("bool IsKeyDown(int)", asFUNCTIONPR([](int a) { return ImGui::IsKeyDown(a); }, (int), bool),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("bool IsKeyPressed(int, bool = true)",
                                 asFUNCTIONPR([](int a, bool b) { return ImGui::IsKeyPressed(a, b); }, (int, bool), bool),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("bool IsKeyReleased(int)",
                                 asFUNCTIONPR([](int a) { return ImGui::IsKeyReleased(a); }, (int), bool), asCALL_CDECL);
  /*engine->RegisterGlobalFunction("int GetKeyPressedAmount(int, float, float)", asFUNCTIONPR([](int a, float b, float c) {  return ImGui::GetKeyPressedAmount(a, b, c);  }, (int,float,float), int), asCALL_CDECL);*/ // FIXME update imgui
  engine->RegisterGlobalFunction("bool IsMouseDown(int)", asFUNCTIONPR([](int a) { return ImGui::IsMouseDown(a); }, (int), bool),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("bool IsMouseClicked(int, bool = false)",
                                 asFUNCTIONPR([](int a, bool b) { return ImGui::IsMouseClicked(a, b); }, (int, bool), bool),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("bool IsMouseDoubleClicked(int)",
                                 asFUNCTIONPR([](int a) { return ImGui::IsMouseDoubleClicked(a); }, (int), bool), asCALL_CDECL);
  engine->RegisterGlobalFunction("bool IsMouseReleased(int)",
                                 asFUNCTIONPR([](int a) { return ImGui::IsMouseReleased(a); }, (int), bool), asCALL_CDECL);
  engine->RegisterGlobalFunction("bool IsMouseDragging(int = 0, float = -1.0f)",
                                 asFUNCTIONPR([](int a, float b) { return ImGui::IsMouseDragging(a, b); }, (int, float), bool),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("bool IsMouseHoveringRect(const ImVec2&in, const ImVec2&in, bool = true)",
                                 asFUNCTIONPR([](const ImVec2 &a, const ImVec2 &b, bool c)
                                              { return ImGui::IsMouseHoveringRect(ImVec2(a.x, a.y), ImVec2(b.x, b.y), c); },
                                              (const ImVec2 &, const ImVec2 &, bool), bool),
                                 asCALL_CDECL);
  /*  engine->RegisterGlobalFunction("bool IsMousePosValid(const ImVec2&in)", asFUNCTIONPR([](const ImVec2& a) {  auto v = ImVec2(a.x, a.y);  return ImGui::IsMousePosValid(&v);  }, (const ImVec2&), bool), asCALL_CDECL); */ // FIXME update imgui
  engine->RegisterGlobalFunction("ImVec2 GetMousePos()",
                                 asFUNCTIONPR(
                                     []()
                                     {
                                       auto v = ImGui::GetMousePos();
                                       return ImVec2(v.x, v.y);
                                     },
                                     (), ImVec2),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("ImVec2 GetMousePosOnOpeningCurrentPopup()",
                                 asFUNCTIONPR(
                                     []()
                                     {
                                       auto v = ImGui::GetMousePosOnOpeningCurrentPopup();
                                       return ImVec2(v.x, v.y);
                                     },
                                     (), ImVec2),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("ImVec2 GetMouseDragDelta(int = 0, float = -1.0f)",
                                 asFUNCTIONPR(
                                     [](int a, float b)
                                     {
                                       auto v = ImGui::GetMouseDragDelta(a, b);
                                       return ImVec2(v.x, v.y);
                                     },
                                     (int, float), ImVec2),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("void ResetMouseDragDelta(int = 0)",
                                 asFUNCTIONPR([](int a) { ImGui::ResetMouseDragDelta(a); }, (int), void), asCALL_CDECL);
  engine->RegisterGlobalFunction("int GetMouseCursor()", asFUNCTIONPR([]() { return ImGui::GetMouseCursor(); }, (), int),
                                 asCALL_CDECL);
  engine->RegisterGlobalFunction("void SetMouseCursor(int)",
                                 asFUNCTIONPR([](ImGuiMouseCursor a) { ImGui::SetMouseCursor(a); }, (int), void), asCALL_CDECL);
  engine->RegisterGlobalFunction("void CaptureKeyboardFromApp(bool = true)",
                                 asFUNCTIONPR([](bool a) { ImGui::CaptureKeyboardFromApp(a); }, (bool), void), asCALL_CDECL);
  engine->RegisterGlobalFunction("void CaptureMouseFromApp(bool = true)",
                                 asFUNCTIONPR([](bool a) { ImGui::CaptureMouseFromApp(a); }, (bool), void), asCALL_CDECL);

  engine->RegisterGlobalFunction("string GetClipboardText()",
                                 asFUNCTIONPR([]() { return string(ImGui::GetClipboardText()); }, (), string), asCALL_CDECL);
  engine->RegisterGlobalFunction(
      "void SetClipboardText(const string&in)",
      asFUNCTIONPR([](const string &a) { ImGui::SetClipboardText(a.empty() ? a.c_str() : 0x0); }, (const string &), void),
      asCALL_CDECL);

  engine->SetDefaultNamespace("");
}
