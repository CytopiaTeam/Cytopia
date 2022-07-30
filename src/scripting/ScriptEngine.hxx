#ifndef SCRIPTENGINE_HXX_
#define SCRIPTENGINE_HXX_

#ifdef USE_ANGELSCRIPT

#include <angelscript.h>
#include <string>
#include <vector>

#include "../util/Singleton.hxx"

enum class ScriptCategory
{
  INVALID,
  BUILD_IN,
  ADDON,
  CUSTOM
};

/// Represents a loaded script and all associated resources/handles.
struct ScriptUnit
{
  ScriptCategory scriptCategory = ScriptCategory::INVALID;
  unsigned int eventMask = 0; ///!< filter mask for script events
  asIScriptModule *scriptModule = nullptr;
  asIScriptFunction *frameStepFunctionPtr = nullptr;            ///!< script function pointer to the frameStep function
  asIScriptFunction *eventCallbackFunctionPtr = nullptr;        ///!< script function pointer to the event callback function
  asIScriptFunction *defaultEventCallbackFunctionPtr = nullptr; ///!< script function pointer for spawner events
  std::string scriptName;
  std::string scriptHash;
};

typedef std::vector<ScriptUnit> ScriptUnitVec;

class ScriptEngine : public Singleton<ScriptEngine>
{
public:
  friend Singleton<ScriptEngine>;

  ScriptEngine() = default;
  ~ScriptEngine();

  /**
    * @brief Setup the ScriptEngine
    *
    */
  void init();

  /**
     * Loads a script
     * @param scriptname filename to load
     * @return 0 on success, everything else on error
     */
  int loadScript(const std::string &scriptname, ScriptCategory category = ScriptCategory::ADDON);

  /**
     * Unloads a script
     * @param scriptname filename to unload
     */
  void unloadScript(const std::string &scriptname, ScriptCategory category);

  /**
     * Calls the script's framestep function to be able to use timed things inside the script
     * @param dt time passed since the last call to this function in seconds
     * @return 0 on success, everything else on error
     */
  int framestep(float dt);

protected:
  /**
    * @brief The message callback used to receive information on errors in human readable form.
    *
    */
  void messageCallback(const asSMessageInfo *msg);

  std::string composeModuleName(std::string const &scriptName, ScriptCategory origin);

  int setupScriptUnit(int unit_id);

  asIScriptEngine *engine{};   ///!< instance of the scripting engine
  asIScriptContext *context{}; ///!< context in which all scripting happens

  ScriptUnitVec m_addon_scrips;
};

void registerImVec2(asIScriptEngine *engine);
void registerImGuiBindings(asIScriptEngine *engine);

#endif
#endif
