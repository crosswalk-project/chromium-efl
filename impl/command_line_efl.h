#ifndef COMMAND_LINE_EFL
#define COMMAND_LINE_EFL

#include "base/command_line.h"
#include "content/public/common/main_function_params.h"

class CommandLineEfl {
public:
  static void Init(int argc, char *argv[]);

  static void Shutdown() {};

  // Get default set of arguments for Tizen port of chromium.
  static content::MainFunctionParams GetDefaultPortParams();

  // Append port specific command line arguments for a specific
  // chromium process type (browser, renderer, zygote, etc).
  static void AppendProcessSpecificArgs(CommandLine& command_line);

  // Chromium content API expects const char**, but EWK API provides
  // us only with char*
  static const char** GetArgv() { return (const char **)argv_; };
  static int GetArgc() { return argc_; };

private:
  static void AppendUserArgs(CommandLine& command_line);

  // Original process argument array provided thorugh EWK API, unfortunately
  // chromium has a nasty habit of messig them up. Please do not rely on those
  // values, use original_arguments_ instead.
  static int argc_;
  static char** argv_;

  // Original arguments passed by the user from command line. Added to all
  // chromium processes. They should remain unchanged until engine shutdown.
  typedef std::vector<std::string> ArgumentVector;
  static ArgumentVector original_arguments_;

  static bool is_initialized_;
};

#endif // COMMAND_LINE_EFL
