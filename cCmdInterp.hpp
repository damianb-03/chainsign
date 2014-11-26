#ifndef CCMDINTERP_HPP
#define CCMDINTERP_HPP

#include <fstream>
#include <string>
#include "ckeysstorage.h"

class cCmdInterp
{
public:
	cCmdInterp() = default;
	cCmdInterp(std::string pFifoName, std::string pInstance);
	void cmdReadLoop();
	unsigned int verify(std::string firstKey);
	void setOutDir(std::string outDir);
private:
	std::ifstream inputFIFO;
	cKeysStorage keyStorage;
	std::string inst;
	std::string mOutDir;
};

#endif
