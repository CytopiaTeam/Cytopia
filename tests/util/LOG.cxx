#include <catch.hpp>
#include "../../src/util/LOG.hxx"
#include "../../src/engine/common/Constants.hxx"
#include "../../src/util/Filesystem.hxx"
#include <fstream>
#include <string>

using std::string;

SCENARIO("Log file never exceed the size limit", "[util]")
{
  GIVEN("Log file's size is greater than the size limit")
  {
    string errfname = CYTOPIA_DATA_DIR + string{"error.log"};
    fs::createDirectory(CYTOPIA_DATA_DIR);
    {
      std::fstream fs(errfname + string("error.log"), std::fstream::trunc | std::fstream::out);
      fs << string(LOG::MAX_LOG_SIZE_BYTES::value, '\n');
      /* fs gets closed */
    }
    WHEN("I log something")
    {
      LOG(LOG_DEBUG) << "I'm logging something";
      THEN("Log file should be cut in half")
      {
        std::fstream fs(errfname + string("error.log"), std::fstream::ate | std::fstream::in);
        std::streampos Size = fs.tellp();
        CHECK(Size != -1);
        CHECK(Size <= LOG::MAX_LOG_SIZE_BYTES::value / 2);
      }
    }
  }
}
