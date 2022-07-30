#include "ScriptEngine.hxx"
#include "LOG.hxx"

// Include the definitions of the script library and the add-ons we'll use.
// The project settings may need to be configured to let the compiler where
// to find these headers. Don't forget to add the source modules for the
// add-ons to your project as well so that they will be compiled into the
// application.

#include <Filesystem.hxx>

#include <angelscript.h>
#include <scriptstdstring/scriptstdstring.h>
#include <scriptarray/scriptarray.h>
#include <scriptdictionary/scriptdictionary.h>
#include <scriptmath/scriptmath.h>
#include <scriptmath/scriptmathcomplex.h>
#include <scriptbuilder/scriptbuilder.h>
#include <cassert>
#include <string>
#include <SDL.h>

using namespace std;

void print(const string &str) { LOG(LOG_INFO) << "[AS] " << str; }

ScriptEngine::~ScriptEngine()
{
  // Clean up
  if (context)
    context->Release();
  if (engine)
    engine->ShutDownAndRelease();
}

void ScriptEngine::init()
{
  // Create the script engine
  engine = asCreateScriptEngine();
  engine->SetEngineProperty(asEP_ALLOW_UNSAFE_REFERENCES, true); // Needed for ImGui

  // Set the message callback to receive information on errors in human readable form.
  int r = engine->SetMessageCallback(asMETHOD(ScriptEngine, messageCallback), this, asCALL_THISCALL);
  assert(r >= 0);

  // AngelScript doesn't have a built-in string type, as there is no definite standard
  // string type for C++ applications. Every developer is free to register its own string type.
  // The SDK do however provide a standard add-on for registering a string type, so it's not
  // necessary to implement the registration yourself if you don't want to.
  RegisterStdString(engine);
  RegisterScriptArray(engine, true);
  RegisterScriptDictionary(engine);
  RegisterScriptMath(engine);
  RegisterScriptMathComplex(engine);

  // Register our own functions
  registerImVec2(engine);
  registerImGuiBindings(engine);

  // Register the function that we want the scripts to call
  r = engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL);
  assert(r >= 0);

  context = engine->CreateContext();
}

int ScriptEngine::framestep(float dt)
{
  // framestep stuff below
  if (!engine || !context)
    return 0;

  for (auto &m_addon_scrip : m_addon_scrips)
  {
    if (!m_addon_scrip.frameStepFunctionPtr)
    {
      continue;
    }

    context->Prepare(m_addon_scrip.frameStepFunctionPtr);

    // Set the function arguments
    context->SetArgFloat(0, dt);

    int r = context->Execute();
    if (r == asEXECUTION_FINISHED)
    {
      // The return value is only valid if the execution finished successfully
      asDWORD ret = context->GetReturnDWord();
    }
  }
  return 0;
}

int ScriptEngine::loadScript(const string &scriptName, ScriptCategory category /* = ScriptCategory::ADDON*/)
{
  // This function creates a new script unit, tries to set it up and removes it if setup fails.
  // -----------------------------------------------------------------------------------------
  // A script unit is how Rigs of Rods organizes scripts from various sources.
  // Because the script is executed during loading, it's wrapping unit must
  // be created early, and removed if setup fails.
  int unit_id = (int)m_addon_scrips.size();
  m_addon_scrips.resize(m_addon_scrips.size() + 1);
  m_addon_scrips[unit_id].scriptName = scriptName;
  m_addon_scrips[unit_id].scriptCategory = category;

  // Perform the actual script loading, building and running main().
  int result = this->setupScriptUnit(unit_id);

  // If setup failed, remove the unit.
  if (result != 0)
  {
    m_addon_scrips.pop_back();
  }

  return result;
}

int ScriptEngine::setupScriptUnit(int unit_id)
{
  int result = 0;
  if (!engine || !context)
    return -2;

  string moduleName = this->composeModuleName(m_addon_scrips[unit_id].scriptName, m_addon_scrips[unit_id].scriptCategory);
  if (moduleName.empty())
    return -1;

  // The builder is a helper class that will load the script file,
  // search for #include directives, and load any included files as
  // well.
  CScriptBuilder builder;

  // A script module is how AngelScript organizes scripts.
  // It contains the script loaded by user plus all `#include`-d scripts.
  result = builder.StartNewModule(engine, moduleName.c_str());
  if (result < 0)
  {
    LOG(LOG_ERROR) << "Could not load script " << moduleName << " - failed to create module.";
    return result;
  }
  m_addon_scrips[unit_id].scriptModule = engine->GetModule(moduleName.c_str(), asGM_ONLY_IF_EXISTS);

  // Load the script from the file system.
  result = builder.AddSectionFromFile(m_addon_scrips[unit_id].scriptName.c_str());
  if (result < 0)
  {
    LOG(LOG_ERROR) << "Could not load script " << moduleName << " - failed to process file.";
    return result;
  }

  result = builder.BuildModule();
  if (result < 0)
  {
    LOG(LOG_ERROR) << "Could not load script " << moduleName << " - failed to build module.";
    return result;
  }

  // get some other optional functions
  m_addon_scrips[unit_id].frameStepFunctionPtr = m_addon_scrips[unit_id].scriptModule->GetFunctionByDecl("void frameStep(float)");

  m_addon_scrips[unit_id].eventCallbackFunctionPtr =
      m_addon_scrips[unit_id].scriptModule->GetFunctionByDecl("void eventCallback(int, int)");

  m_addon_scrips[unit_id].defaultEventCallbackFunctionPtr =
      m_addon_scrips[unit_id].scriptModule->GetFunctionByDecl("void defaultEventCallback(int, string, string, int)");

  // Find the function that is to be called.
  auto main_func = m_addon_scrips[unit_id].scriptModule->GetFunctionByDecl("void main()");
  if (main_func == nullptr)
  {
    // The function couldn't be found. Instruct the script writer to include the
    // expected function in the script.
    LOG(LOG_ERROR) << "Could not load script " << moduleName << " - there is no function `main()";
    return 0;
  }

  // Prepare the script context with the function we wish to execute. Prepare()
  // must be called on the context before each new script function that will be
  // executed. Note, that if you intend to execute the same function several
  // times, it might be a good idea to store the function id returned by
  // GetFunctionIDByDecl(), so that this relatively slow call can be skipped.
  result = context->Prepare(main_func);
  if (result < 0)
  {
    LOG(LOG_ERROR) << "Could not load script " << moduleName << " - failed to build module.";
    context->Release();
    return -1;
  }

  // Execute the `main()` function in the script.
  // The function must have full access to the game API.
  LOG(LOG_DEBUG) << "Executing main() in " << moduleName;
  result = context->Execute();
  if (result != asEXECUTION_FINISHED)
  {
    // The execution didn't complete as expected. Determine what happened.
    if (result == asEXECUTION_ABORTED)
    {
      LOG(LOG_DEBUG) << "The script was aborted before it could finish. Probably it timed out.";
    }
    else if (result == asEXECUTION_EXCEPTION)
    {
      // An exception occurred, let the script writer know what happened so it can be corrected.
      LOG(LOG_ERROR) << "An exception '" << context->GetExceptionString() << "' occurred. Please correct the code in file '"
                     << m_addon_scrips[unit_id].scriptName << "' and try again.";

      // Write some information about the script exception
      asIScriptFunction *func = context->GetExceptionFunction();
      LOG(LOG_DEBUG) << "func: " << func->GetDeclaration();
      LOG(LOG_DEBUG) << "modl: " << func->GetModuleName();
      LOG(LOG_DEBUG) << "sect: " << func->GetScriptSectionName();
      LOG(LOG_DEBUG) << "line: " << context->GetExceptionLineNumber();
      LOG(LOG_DEBUG) << "desc: " << context->GetExceptionString();
    }
    else
    {
      LOG(LOG_DEBUG) << "The script ended for some unforeseen reason " << result;
    }

    LOG(LOG_ERROR) << "Could not load script " << moduleName << " - error running function `main()`, check AngelScript.log";
  }
  else
  {
    LOG(LOG_DEBUG) << "The script finished successfully.";
  }

  return 0;
}

void ScriptEngine::unloadScript(const string &scriptName, ScriptCategory category)
{
  string module_name = this->composeModuleName(scriptName, category);
  if (module_name.empty())
    return;

  for (size_t i = 0; i < m_addon_scrips.size(); i++)
  {
    if (m_addon_scrips[i].scriptModule->GetName() == module_name)
    {
      m_addon_scrips.erase(m_addon_scrips.begin() + i);
    }
  }
}

string ScriptEngine::composeModuleName(string const &scriptName, ScriptCategory origin)
{
  switch (origin)
  {
  case ScriptCategory::BUILD_IN:
    return "BUILD_IN: " + scriptName;

  case ScriptCategory::ADDON:
    return "ADDON: " + scriptName;

  case ScriptCategory::CUSTOM:
    return "CUSTOM: " + scriptName;

  default:
    return "";
  }
}

void ScriptEngine::messageCallback(const asSMessageInfo *msg)
{
  switch (msg->type)
  {
  case asMSGTYPE_INFORMATION:
    LOG(LOG_INFO) << msg->section << " (" << msg->row << "," << msg->col << ") " << msg->message;
    break;

  case asMSGTYPE_WARNING:
    LOG(LOG_WARNING) << msg->section << " (" << msg->row << "," << msg->col << ") " << msg->message;
    break;

  case asMSGTYPE_ERROR:
    LOG(LOG_ERROR) << msg->section << " (" << msg->row << "," << msg->col << ") " << msg->message;
    break;

  default:
    LOG(LOG_DEBUG) << msg->section << " (" << msg->row << "," << msg->col << ") " << msg->message;
    break;
  }
}
