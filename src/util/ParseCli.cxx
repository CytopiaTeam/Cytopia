#include "ParseCli.hxx"
#include "LOG.hxx"
#include "Singleton.hxx"
#include "Settings.hxx"

// option identifiers
enum
{
  OPT_HELP,
  OPT_SKIPMENU,
  OPT_VIDEODRIVER
};

// option array
CSimpleOpt::SOption cmdline_options[] = {{OPT_SKIPMENU, ("--skipMenu"), SO_NONE},
                                         {OPT_VIDEODRIVER, ("--video"), SO_REQ_SEP},
                                         {OPT_HELP, ("--help"), SO_NONE},
                                         {OPT_HELP, ("-h"), SO_NONE},
                                         SO_END_OF_OPTIONS};

bool ParseCli::ProcessCommandLine(int argc, char *argv[])
{
  CSimpleOpt args(argc, argv, cmdline_options);
  bool success = true;
  while (args.Next())
  {
    if (args.LastError() != SO_SUCCESS)
    {
      LOG(LOG_ERROR) << GetLastErrorText(args.LastError()) << " " << args.OptionText();
      success = false;
    }

    switch (args.OptionId())
    {
    case OPT_HELP:
      ShowUsage();
      return false;
    case OPT_SKIPMENU:
      Settings::instance().skipMenu = true;
      break;
    case OPT_VIDEODRIVER:
      if (args.OptionArg())
      {
        Settings::instance().videoDriver = args.OptionArg();
      }
      else
      {
        LOG(LOG_ERROR) << "videoDriver not set";
        ShowUsage();
        return false;
      }
      break;
    default:
      ShowUsage();
    }
  }

  return success;
}

void ParseCli::ShowUsage()
{
  LOG(LOG_INFO) << "Usage: Cytopia [OPTIONS]";
  LOG(LOG_INFO) << "\t--help (this)";
  LOG(LOG_INFO) << "\t--skipMenu (Skips the main menu)";
  LOG(LOG_INFO) << "\t--video <videoDriver> (Sets a different video driver)";
}

std::string ParseCli::GetLastErrorText(int a_nError)
{
  switch (a_nError)
  {
  case SO_SUCCESS:
    return "Success";
  case SO_OPT_INVALID:
    return "Unrecognized option";
  case SO_OPT_MULTIPLE:
    return "Option matched multiple strings";
  case SO_ARG_INVALID:
    return "Option does not accept argument";
  case SO_ARG_INVALID_TYPE:
    return "Invalid argument format";
  case SO_ARG_MISSING:
    return "Required argument is missing";
  case SO_ARG_INVALID_DATA:
    return "Invalid argument data";
  default:
    return "Unknown error";
  }
}