#ifndef SCRIPTENGINE_HXX_
#define SCRIPTENGINE_HXX_

#ifdef USE_ANGELSCRIPT

#include <angelscript.h>
#include <string>

#include "../util/Singleton.hxx"

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

#endif
