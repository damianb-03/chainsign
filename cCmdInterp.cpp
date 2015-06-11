#include "cCmdInterp.hpp"
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>
#include <boost/filesystem.hpp>

#define KEY_SIZE 2048

cCmdInterp::cCmdInterp(std::string pFifoName) {
	inputFIFO.open(pFifoName);
}

void cCmdInterp::cmdReadLoop()
{
	std::string line;
	std::cout << "start cmdReadLoop" << std::endl;
	std::cout << "mOutDir = " << mOutDir << std::endl;
	mOutDir.clear();
	keyStorage.GenerateRSAKey(KEY_SIZE, mOutDir + "key_" + std::to_string(keyStorage.getCurrentKey()) + ".pub"); // generate 1st key
	while (1)
	{
        std::cout << "loop" << std::endl;
        // read command from fifo
		inputFIFO.open("fifo");
		std::getline(inputFIFO, line);
        //std::cout << "line " << line << std::endl;
		inputFIFO.close();
		if (line == "QUIT")
			break;
		else if(line == "SIGN-NEXTKEY")
		{
			std::cout << "SIGN-NEXTKEY" << std::endl;
			std::cout << "current key: " << keyStorage.getCurrentKey() << std::endl;
			std::cout << std::endl;
			std::string pubFileName = "key_" + std::to_string(keyStorage.getCurrentKey()) + ".pub";
			//std::string nextPubFileName = inst + "-key" + std::to_string(keyStorage.getCurrentKey() + 1) + ".pub";
			std::string path; // input dir
			//system(std::string("touch " + pubFileName).c_str());
			std::cout << "pubFileName " << pubFileName << std::endl;
			
			// get filename
			inputFIFO.open("fifo");
			std::getline(inputFIFO, line);
			if (!boost::filesystem::exists(line)) 
			{
				std::cout << "No found " << line << std::endl;
				//system("rm *.pub");
				continue;
			}
			
			std::cout << "sign file " << line << std::endl;
			//std::cout << "cp " << line << " to ." << std::endl;
			//system(std::string("cp " + line + " .").c_str());
			auto it = line.end();
			while (*it != '/')
				it--;
			auto it2 = line.begin();
			while (it2 != it + 1)
			{
				path += *it2;
				it2++;
			}
			std::cout << "path: " << path << std::endl;
			line.erase(line.begin(), it + 1);
			//std::string outDir = line;
			std::string file(line);
			std::cout << "FILE: " << file << std::endl;
			//outDir.erase(outDir.end() - 4, outDir.end());
			//std::cout << "outDir " << outDir << std::endl;
			//setOutDir(outDir);
			std::cout << "current file: " << line << std::endl;
			//system(std::string("cp " + line + " " + outDir).c_str()); // cp file to out dir(archive)
			std::cout << "out path" << mOutDir + "-" + line + ".sig" << std::endl;
			std::cout << "current key: " << keyStorage.getCurrentKey() << std::endl;
			system(std::string("cp " + line + " .").c_str()); // cp file co current dir
			
			std::cout << "Sign last key" << std::endl;
			std::cout << "Last key name: " << pubFileName << std::endl;
			std::cout << "current key: " << keyStorage.getCurrentKey() << std::endl;
			
			keyStorage.GenerateRSAKey(KEY_SIZE, mOutDir + pubFileName); // XXX
			//keyStorage.RSASignFile(file, mOutDir + "-" + file + ".sig", false); // sign file
			//keyStorage.RSASignFile(mOutDir + pubFileName, mOutDir + pubFileName + ".sig", true);	// sign key
			keyStorage.RSASignNormalFile(file, file + ".sig"); // sign file
			keyStorage.RSASignNormalFile(pubFileName, pubFileName + ".sig");
			
			//keyStorage.GenerateRSAKey(KEY_SIZE, mOutDir + pubFileName); // XXX
			keyStorage.RemoveRSAKey(); // XXX
			//system(std::string("mv *.sig2 " + mOutDir).c_str());
			//system(std::string("cp *.sig2 " + mOutDir).c_str());
			//system(std::string("cp *.sig " + path).c_str());
			//system(std::string("cp *.pub " + path).c_str());
			//system("rm *.pub");
			
			//std::cout << "outDir " << outDir << std::endl;
			//std::cout << "create archive" << std::endl;
			//std::cout << std::string("tar zcvf " + path +  outDir + ".tar.gz " + outDir) << std::endl;
			//system(std::string("tar zcvf " + path + outDir + ".tar.gz " + outDir).c_str());
		}
/*		else if(line == "VERIFY-FILE")
		{
			bool ok;
			std::cout << "VERIFY-FILE" << std::endl;
			std::cout << std::endl;
			inputFIFO.close();
			inputFIFO.open("fifo");
			std::getline(inputFIFO, line);

			std::cout << "line " << line << std::endl;
			std::string tmp;
			unsigned int key; // file was signed this key
			std::ifstream inFile(line);
			inFile >> tmp;
			inFile >> tmp;
			std::cout << "tmp before stoi" << tmp << std::endl;
			tmp.erase(0, 4); // 1.prv
			tmp.erase(tmp.size() - 4); // 1
			key = std::stoi(tmp);
			
			inFile.close();
			if (key >= verify(std::string("key_1.pub"))) // XXX key1.pub.sig
			//if (key < verify(std::string(instance + "-key1.pub"))) // XXX 
			{
				std::cout << "                                               Keys OK" << std::endl;
				ok = true;
			}
			else
			{
				ok = false;
				std::cout << "                                               Keys verification error" << std::endl;
			}
			
			//bool fileOK = keyStorage.RSAVerifyFile(line);
			bool fileOK = keyStorage.RSAVerifyNormalFile(line, line + ".sig");
			inputFIFO.close();
			
			std::cout << "keys " << ok << std::endl;
			std::cout << "file " << fileOK << std::endl;
			if (ok && fileOK)
				std::cout << "File OK" << std::endl;
		}*/
		else if(line == "SIGN-NEXTKEY-WAV-FILES")
		{
			std::cout << "SIGN-NEXTKEY-WAV-FILES" << std::endl;
			boost::filesystem::directory_iterator dirIterator(".");
			boost::filesystem::directory_iterator endIterator;
			//generate new key
			std::string pubFileName =  "key_" + std::to_string(keyStorage.getCurrentKey()) + ".pub";
			system(std::string("touch " + pubFileName).c_str());
			std::cout << "pubFileName " << pubFileName << std::endl;
			while (dirIterator != endIterator)
			{
				if (boost::filesystem::is_regular_file(dirIterator->status()))
				{
					std::stringstream ss;
					std::string fileName;
					ss << *dirIterator;
					ss >> fileName;
					fileName.erase(fileName.begin(), fileName.begin() + 3);
					fileName.pop_back();
					//if (fileName.find("txt") == (fileName.size() - 3)) // XXX wav
					if (fileName.find("wav") == (fileName.size() - 3))
					{
						std::cout << fileName << std::endl;
						// sign
						keyStorage.RSASignFile(fileName, mOutDir + "-" + fileName + ".sig", false);
					}
				}
				
				dirIterator++;
			}
			std::cout << "generate new key" << std::endl;
			keyStorage.GenerateRSAKey(KEY_SIZE, mOutDir + pubFileName);
			std::cout << "rm old key" << std::endl;
			keyStorage.RemoveRSAKey();
			keyStorage.RSASignFile(pubFileName, mOutDir + pubFileName + ".sig", true);	// sign key
			
			//std::cout << "tar cf wav_files.tar " + inst + "*" << std::endl;
			system(std::string("mv *.pub " + mOutDir).c_str());
			system(std::string("mv *.sig2 " + mOutDir).c_str());
			system(std::string("mv *.wav " + mOutDir).c_str());
			//system(std::string("mv *.txt " + mOutDir).c_str());
			//system(std::string("tar czf " + inst + ".tar.gz " + mOutDir).c_str());
			//system(std::string().c_str());
		}
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
    //std::cout << "loop end" << std::endl;
}


unsigned int cCmdInterp::verify(std::string firstKey) // verify keys, get name of 1st pub file, return last good key
{
	std::cout << "start verify keys" << std::endl;
	//std::ifstream pubFile;
	//system(std::string("cp " + firstKey + " " + mOutDir + firstKey).c_str()); // copy 1st key to out dir
/*	
	std::string fileName = instance;
	bool good = true;
	int keyNumber = 2;
	while (firstKey.front() != '-')
	{
		instance += firstKey.front();
		firstKey.erase(firstKey.begin());
	}
*/
	const std::string prefixKeyName("key_"); // key_1.pub, key_2.pub ...
	const std::string suffixKeyName(".pub");
	bool good = true;
	int keyNumber = 2;
	std::string fileName;
	
	std::cout << "start loop" << std::endl; 
	unsigned int lastGoodKey = 1;
	while (good) {
		fileName = prefixKeyName + std::to_string(keyNumber) + suffixKeyName;
		std::cout << "file name " << fileName << std::endl;
		std::ifstream pubFile;
		pubFile.open(fileName);
		if(!pubFile.good()) // no file
		{
			std::cout << "No found " << fileName << std::endl;
			break;
		}
		
		std::cout << "start verify " << fileName << std::endl;
		//good = keyStorage.RSAVerifyFile(fileName);
		good = keyStorage.RSAVerifyNormalFile(fileName, fileName + ".sig");
		//RSAVerifyNormalFile(const std::string& inputFilename, const std::string& signatureFilename);
		if (good) {
			lastGoodKey = keyNumber;
			//std::cout << "mv cmd " << "mv " + fileName + " " + mOutDir + fileName << std::endl;
			//fileName.erase(fileName.end() - 4, fileName.end()); // rm ".sig"
			//system(std::string("cp " + fileName + " " + mOutDir + fileName).c_str());
		}
		keyNumber++;
	}
	
	std::cout << "Last good key: " << lastGoodKey << std::endl;
	if (lastGoodKey > 1)
	{
		std::cout << "OK" << std::endl;
		return lastGoodKey;
	}
	else
		return -1;
}

void cCmdInterp::setOutDir(std::string outDir)
{
	mOutDir = outDir;
	system(std::string("mkdir " + mOutDir).c_str());
	if (outDir.back() != '/')
		mOutDir.push_back('/');
	//std::cout << "out dir: " << mOutDir << std::endl;
}

unsigned int cCmdInterp::verifyOneFile(std::string fileName) //fileName = sig file
{
	std::string instance;
	auto it = fileName.begin();
	while (*it != '-')
	{
		instance.push_back(*it);
		it++;
	}
	
	//std::cout << "RSAVerifyFile " << keyStorage.RSAVerifyFile(fileName, instance) << std::endl;
	
	std::string firstPubKey;
	firstPubKey = instance + "-key1.pub";
	
	std::cout << "Start keys verification" << std::endl;
	std::cout << "first pub key " << firstPubKey << std::endl;
	unsigned int ret = verify(std::string(fileName));
	if (ret == -1)
		return 2;
	
	std::cout << "file name " << fileName << std::endl;
	ret = keyStorage.RSAVerifyFile(fileName);
	//std::cout << ret << std::endl;
	if (ret == 0)
		return 3;
	
	std::cout << "OK" << std::endl;	
	return 0;
}
