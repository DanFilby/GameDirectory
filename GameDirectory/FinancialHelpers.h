#pragma once
#include "Common.h"
#include "Database.h"

#include <sstream>

class FinancialTermDescription : Database{
	inline static const string FINANCIAL_TERMS_FILE_PATH = APP_DATA_DIR_PATH + "Financial-Terms.dat";
	inline static const char LINE_DELIM = ':';

public:
	static string GetDescription(string financialTerm){
		string description;
		if (!FileExsits(FINANCIAL_TERMS_FILE_PATH) || !IsValidFinancialTerm(financialTerm)
			|| !TermExsists(financialTerm, description)) {
			return "definition not found"; 
		}
		return description;
	}

	static void AddDescription(string financialTerm, string definition) {
		if (!IsValidFinancialTerm(financialTerm)) { return; }

		SetTermToFileFormat(financialTerm);
		SetDefinitionToFileFormat(definition);

		ofstream writeFile = ofstream(FINANCIAL_TERMS_FILE_PATH, std::ios::out | std::ios::app);
		
		std::stringstream ss{};
		ss << financialTerm << LINE_DELIM << definition << '\n';
		writeFile.write(ss.str().c_str(), ss.str().size());
	}

private:
	static bool TermExsists(string& financialTerm, string & out_TermDefinition) {
		SetTermToFileFormat(financialTerm);

		ifstream financialTermsFile = ifstream(FINANCIAL_TERMS_FILE_PATH, std::ios::in);

		string lineBuf, fileTermBuf;

		if (financialTermsFile.is_open()) {
			while (financialTermsFile.good()) {
				std::getline(financialTermsFile, lineBuf);
				if (lineBuf.empty()) { continue; }

				std::stringstream streamBuffer(lineBuf);
				getline(streamBuffer, fileTermBuf, LINE_DELIM);

				if (financialTerm == fileTermBuf) {
					getline(streamBuffer, out_TermDefinition, '\n');
					return true;
				}
			}
		}
		return "definition not found";
	}

	static bool TermExsists(string& financialTerm) {
		string discard;
		return TermExsists(financialTerm, discard);
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

	static void SetDefinitionToFileFormat(string & definition) {
		//check for commas and new lines

	}

};
