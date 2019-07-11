#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <getopt.h>

struct Config {
  string from_temp_path;
  string to_temp_path;

  string input_path;
  string output_path;

  int initial_chunksize;
  int merge_concurrency;
};

inline Config parseOptions(int argc, char* argv[]) {
  Config cfg;

  // defaults
  cfg.initial_chunksize = 32768;
  cfg.merge_concurrency = 8192;

  char ch;

  static option long_options[] = {
    {"input",             required_argument, 0,  'i' },
    {"output",            required_argument, 0,  'o' },
    {"merge-concurrency", required_argument, 0,  'm' },
    {"chunksize",         required_argument, 0,  'c' },
    {"help",              no_argument,       0,  'h' },
    {0,                   0,                 0,  0 }
  };
  static const char *opts = "hi:o:m:c:";

  while ((ch = getopt_long(argc, argv, opts, long_options, NULL)) != -1) {
    switch (ch) {
    case 'h':
      std::cout << "Help:" << std::endl;
      std::cout << "--input, -i:\tinput filename path" << std::endl;
      std::cout << "--output, -o:\tinput filename path" << std::endl;
      std::cout << "--merge-concurrency, -m:\tmaximum number of chunks to be merged in parallel (1024 by default)" << std::endl;
      std::cout << "--chunksize, -c:\tinitial chunks minimum size (1024 bytes by default)" << std::endl;

      exit(0);
      break;

    case 'i': cfg.input_path = optarg; break;
    case 'o': cfg.output_path = optarg; break;
    case 'm': cfg.merge_concurrency = atoi(optarg); break;
    case 'c': cfg.initial_chunksize = atoi(optarg); break;

    default:
      std::cout << "invalid option provided" << std::endl;
      break;
    }
  }

  if (cfg.output_path == "") {
    cfg.output_path = cfg.input_path;
  }

  return cfg;
}

#endif
