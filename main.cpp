#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

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
    std::cerr << "Invalid arguments\n";
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
  } catch (std::ios_base::failure& e) {
    std::cerr << "Failed to open your SND file\n";
    return 0;
  }

  try {
    source.open(argv[3], std::ios::binary);
  } catch (std::ios_base::failure& e) {
    std::cerr << "Failed to load your modified WEM file\n";
    return 0;
  }

  source.seekg(0, std::ios::end);
  uint32_t sourceSize = source.tellg();
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
  uint32_t lenPos = file.tellg();
  lenPos -= 20;

  file.seekg(0, std::ios::end);
  uint32_t newOff = file.tellg();
  file.close();

  std::ofstream ofile(argv[1], std::ios::binary | std::ios::in | std::ios::out);
  ofile.seekp(lenPos, std::ios_base::beg);
  ofile.write(reinterpret_cast<const char*>(&sourceSize), sizeof(sourceSize));
  ofile.write(reinterpret_cast<const char*>(&newOff), sizeof(newOff));
  ofile.write(reinterpret_cast<const char*>(&sourceSize), sizeof(sourceSize));
  ofile.seekp(0, std::ios_base::end);

  try {
    while (!source.eof()) {
      char ch = source.get();
      ofile.write(&ch, 1);
    }
  } catch (...) {}

  ofile.close();
  source.close();
  std::cout << "WEM file successfully injected into SND\n";
}
