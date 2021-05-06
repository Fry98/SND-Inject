#include <iostream>
#include <fstream>

#ifdef _WIN32
  #define cmdBeg "opusdec.exe "
  #define cmdBegLen 12
#else
  #include <cstring>
  #define cmdBeg "wine opusdec.exe "
  #define cmdBegLen 17
#endif

struct Entry {
  uint64_t idk;
  uint32_t id;
  uint32_t length;
  uint32_t offset;
  uint32_t uncompLength;
  uint64_t idc;
};

int main(int argc, char* argv[]) {
  if (argc != 4) {
    std::cerr << "Invalid arguments\n\n";
    std::cerr << "Usage:\n";
    std::cerr << argv[0] << " [path_to_SND_archive] [SFX_ID] [path_to_your_audio_file]\n";
    return 0;
  }

  errno = 0;
  char *endptr;
  long int x = strtol(argv[2], &endptr, 10);
  if (endptr == argv[2] || *endptr || errno == ERANGE || x < 0) {
    std::cerr << "Invalid SFX ID\n";
    return 0;
  }

  uint32_t id = x;
  std::ifstream file;
  std::ifstream source;

  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  source.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    file.open(argv[1], std::ios::binary);
  } catch (...) {
    std::cerr << "Failed to open your SND file\n";
    return 0;
  }

  uint32_t uncompLen = 0;
  size_t strLen = strlen(argv[3]);
  bool opus = false;

  if (strcmp(argv[3] + strLen - 3, "ogg") == 0 || strcmp(argv[3] + strLen - 4, "opus") == 0 ) {
    opus = true;
  } else if (strcmp(argv[3] + strLen - 3, "wem") != 0) {
    std::cerr << "Unsupported audio format\n";
    return 0;
  }

  try {
    source.open(argv[3], std::ios::binary);
  } catch (...) {
    if (opus) {
      std::cerr << "Failed to load your modified Opus file\n";
      return 0;
    }
    std::cerr << "Failed to load your modified WEM file\n";
    return 0;
  }

  if (opus) {
    size_t cmdLen = 21 + cmdBegLen + strLen;
    char* cmd = new char[cmdLen];

    strcpy_s(cmd, cmdLen, cmdBeg);
    strcat_s(cmd, cmdLen, argv[3]);
    strcat_s(cmd, cmdLen, " temp.wav >nul 2>nul");
    system(cmd);

    std::ifstream uncompFile;
    uncompFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
      uncompFile.open("temp.wav", std::ifstream::ate | std::ifstream::binary);
    } catch (...) {
      std::cerr << "Opus decompression failed\n";
      return 0;
    }

    uncompLen = (uint32_t) uncompFile.tellg();
    uncompLen += 20;
    uncompFile.close();
    remove("temp.wav");
  }

  source.seekg(0, std::ios::end);
  uint32_t sourceSize = (uint32_t) source.tellg();
  source.seekg(0, std::ios::beg);

  uint32_t infoSize;
  uint32_t headSize;

  file.seekg(4, std::ios::beg);
  file.read(reinterpret_cast<char *>(&infoSize), sizeof(infoSize));
  file.read(reinterpret_cast<char *>(&headSize), sizeof(headSize));

  infoSize += 4;
  file.seekg(headSize, std::ios::cur);

  Entry curEntr;
  while (file.tellg() < infoSize) {
    file.read(reinterpret_cast<char *>(&curEntr), sizeof(curEntr));
    if (curEntr.id == id) goto found;
  }

  std::cerr << "Could not find the specified SFX ID\n";
  return 0;

  found:
  uint32_t lenPos = (uint32_t) file.tellg();
  lenPos -= 20;

  file.seekg(0, std::ios::end);
  uint32_t newOff = (uint32_t) file.tellg();
  file.close();

  std::ofstream ofile(argv[1], std::ios::binary | std::ios::in | std::ios::out);
  ofile.seekp(lenPos, std::ios_base::beg);
  ofile.write(reinterpret_cast<const char*>(&sourceSize), sizeof(sourceSize));
  ofile.write(reinterpret_cast<const char*>(&newOff), sizeof(newOff));
  ofile.write(reinterpret_cast<const char*>(&(opus ? uncompLen : sourceSize)), sizeof(uint32_t));
  ofile.seekp(0, std::ios_base::end);

  try {
    while (!source.eof()) {
      char ch = source.get();
      ofile.write(&ch, 1);
    }
  } catch (...) {}

  ofile.close();
  source.close();

  if (opus) {
    std::cout << "Opus file successfully injected into SND\n";
  } else {
    std::cout << "WEM file successfully injected into SND\n";
  }
}
