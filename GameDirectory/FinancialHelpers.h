#pragma once
#include "Common.h"
#include "Database.h"

#include <sstream>

class FinancialTermDescription : Database{
	inline static const string FINANCIAL_TERMS_FILE_PATH = APP_DATA_DIR_PATH + "Financial-Terms.dat";
	inline static const char LINE_DELIM = ':';

public:
	//TODO: add functionality to replace definition

	static string GetDescription(string financialTerm){
		string description;
		if (!FileExsits(FINANCIAL_TERMS_FILE_PATH) || !IsValidFinancialTerm(financialTerm)
			|| !TermExsists(financialTerm, description)) {
			return "definition not found"; 
		}
		return description;
	}

	static void AddDescription(string financialTerm, string definition) {
		if (!IsValidFinancialTerm(financialTerm) || TermExsists(financialTerm)) { return; }

		SetTermToFileFormat(financialTerm);
		SetDefinitionToFileFormat(definition);

		ofstream writeFile = ofstream(FINANCIAL_TERMS_FILE_PATH, std::ios::out | std::ios::app);
		
		std::stringstream ss{};
		ss << financialTerm << LINE_DELIM << definition << '\n';
		writeFile.write(ss.str().c_str(), ss.str().size());
	}

	static void EditDescription(string financialTerm, string definition) {
		if (!IsValidFinancialTerm(financialTerm) || !TermExsists(financialTerm)) { return; }

		SetTermToFileFormat(financialTerm);
		SetDefinitionToFileFormat(definition);
		
		RemoveTermFromFile(financialTerm);

		AddDescription(financialTerm, definition);
	}

private:
	static bool TermExsists(string& financialTerm, string & out_TermDefinition) {
		SetTermToFileFormat(financialTerm);

		ifstream financialTermsFile = ifstream(FINANCIAL_TERMS_FILE_PATH, std::ios::in);
		if (!financialTermsFile.is_open()) { return false; }

		string lineBuf, fileTermBuf;

		while (financialTermsFile.good()) {
			std::getline(financialTermsFile, lineBuf);
			if (lineBuf.empty()) { continue; }

			std::stringstream streamBuffer(lineBuf);
			getline(streamBuffer, fileTermBuf, LINE_DELIM);

			if (financialTerm == fileTermBuf || financialTerm == ToCamalCase(fileTermBuf)) {
				getline(streamBuffer, out_TermDefinition, '\n');
				return true;
			}
		}
		
		return false;
	}

	static bool TermExsists(string& financialTerm) {
		string discard;
		return TermExsists(financialTerm, discard);
	}

	static void RemoveTermFromFile(string financialTerm) {
		if (!TermExsists(financialTerm)) { return; }

		ifstream financialTermsFile = ifstream(FINANCIAL_TERMS_FILE_PATH, std::ios::in);
		if (!financialTermsFile.is_open()) { return; }
		std::stringstream fileContents;

		string lineBuf, fileTermBuf;	

		while (financialTermsFile.good()) {
			std::getline(financialTermsFile, lineBuf);
			if (lineBuf.empty()) { continue; }

			//read financial term from file line
			std::stringstream streamBuffer(lineBuf);
			getline(streamBuffer, fileTermBuf, LINE_DELIM);

			if (financialTerm == fileTermBuf || financialTerm == ToCamalCase(fileTermBuf)) { continue; }
			else { fileContents << string(lineBuf) << "\n"; }
		}
		financialTermsFile.close();

		ofstream financialTermsNewFile = ofstream(FINANCIAL_TERMS_FILE_PATH, std::ios::out | std::ios::trunc);
		financialTermsNewFile << fileContents.rdbuf();
	}

	static bool IsValidFinancialTerm(const string& financialTerm) {
		if (financialTerm.size() <= 0 || financialTerm.size() >= 32) { return false; }
	}

	static void SetTermToFileFormat(string& financialTerm) {
		string tempString = std::move(financialTerm);

		for (char c : tempString) {
			financialTerm.append(1,(char)std::tolower(c));
		}

		std::replace(financialTerm.begin(), financialTerm.end(), ' ', '_');
	}

	static string ToCamalCase(string str) {
		//not exactly to camal case, but to compare against a camal case financial term input
		str.erase(std::remove(str.begin(), str.end(), '_'), str.end());
		return str;
	}

	static void SetDefinitionToFileFormat(string & definition) {
		if (definition.size() > 512) { definition.resize(512); }

		std::replace(definition.begin(), definition.end(), LINE_DELIM, '-');
		std::replace(definition.begin(), definition.end(), '\n', '.');
	}

};
