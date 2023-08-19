#pragma once
#include "Common.h"

#include "Database.h"

class FinancialTermDescription : Database{
	inline static const string FINANCIAL_TERMS_FILE_PATH = APP_DATA_DIR_PATH + "Financial-Terms.dat";

public:
	static string GetDescription(string financialTerm){
		if (!FileExsits(FINANCIAL_TERMS_FILE_PATH)) { return "definition not found\n"; }
		return SearchFileForDefinition();
	}

	static void AddDescription(string financialTerm, string definition) {

	}


private:
	static string SearchFileForDefinition(string financialTerm) {


	}



};
