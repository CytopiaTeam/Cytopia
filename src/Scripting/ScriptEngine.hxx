#ifndef SCRIPTENGINE_HXX_
#define SCRIPTENGINE_HXX_

#include <angelscript.h>
#include <string>

class ScriptEngine
{
public:

  ScriptEngine();
  ~ScriptEngine();
  void init();
  void MessageCallback(const asSMessageInfo *msg);

protected:
  asIScriptEngine *engine;                            //!< instance of the scripting engine
  asIScriptContext *context;                          //!< context in which all scripting happens

};

#endif