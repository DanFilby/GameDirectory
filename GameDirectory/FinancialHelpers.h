#pragma once
#include "Common.h"
#include "Database.h"

#include <sstream>

class FinancialTermDescription : Database{
	inline static const string FINANCIAL_TERMS_FILE_PATH = APP_DATA_DIR_PATH + "Financial-Terms.dat";

public:
	static string GetDescription(string financialTerm){
		if (!FileExsits(FINANCIAL_TERMS_FILE_PATH) || !IsValidFinancialTerm(financialTerm)) {
			return "definition not found"; 
		}
		return SearchFileForDefinition(financialTerm);
	}

	static void AddDescription(string financialTerm, string definition) {
		//check for commas and new lines
	}


private:
	static string SearchFileForDefinition(string& financialTerm) {
		SetTermToFileFormat(financialTerm);
		
		ifstream financialTermsFile = ifstream(FINANCIAL_TERMS_FILE_PATH, std::ios::in);

		string lineBuf, fileTermBuf;

		if (financialTermsFile.is_open()) {
			while (financialTermsFile.good()) {
				std::getline(financialTermsFile, lineBuf);
				if (lineBuf.empty()) { continue; }

				std::stringstream streamBuffer(lineBuf);
				getline(streamBuffer, fileTermBuf, ',');

				if (std::strcmp(financialTerm.c_str(), fileTermBuf.c_str())) {
					string termDefinition; getline(streamBuffer, termDefinition, ',');
					return termDefinition;
				}		
			}
		}
		return "definition not found";
	}

	static bool IsValidFinancialTerm(const string& financialTerm) {
		if (financialTerm.size() <= 0 || financialTerm.size() >= 25) { return false; }
	}

	static void SetTermToFileFormat(string& financialTerm) {
		string tempString = std::move(financialTerm);

		for (char c : tempString) {
			financialTerm.append(1,(char)std::tolower(c));
		}
	}

};
