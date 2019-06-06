#ifndef SCRIPTENGINE_HXX_
#define SCRIPTENGINE_HXX_

#include <angelscript.h>
#include <string>

class ScriptEngine
{
public:
    
  /// Retrieves instance of Singleton class UI Manager
  static ScriptEngine &instance()
  {
    static ScriptEngine se;
    return se;
  }

  ScriptEngine() = default;
  ~ScriptEngine();

  /**
    * @brief Setup the ScriptEngine
    *
    */
  void init();

protected:
  /**
    * @brief The message callback used to receive information on errors in human readable form.
    *
    */
  void MessageCallback(const asSMessageInfo *msg);

  asIScriptEngine *engine{};   //!< instance of the scripting engine
  asIScriptContext *context{}; //!< context in which all scripting happens
};

#endif