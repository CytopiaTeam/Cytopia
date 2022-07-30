#include <catch.hpp>
#include "ScriptEngine.hxx"
#include "Filesystem.hxx"

TEST_CASE("The test script can be loaded", "[ScriptEngine]")
{
  ScriptEngine &scriptEngine = ScriptEngine::instance();
  scriptEngine.init();
  auto res = scriptEngine.loadScript(fs::getBasePath() + "/resources/test.as", ScriptCategory::BUILD_IN);
  CHECK(res == 0);
}