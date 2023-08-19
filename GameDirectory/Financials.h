#pragma once
#include "Common.h"
#include "FinancialHelpers.h"

#include <sstream>

//share
//quater
//market cap
//balance sheet 
//cash flow

enum Quater : uint8_t { None = 0, Q1 = 1, Q2 = 2, Q3 = 3, Q4 = 4 };

/// <summary>
/// Functionality to factor money into billions, millions etc
/// </summary>
struct MoneyFactoring {
	enum MoneyFactor { Billions = 0, Millions = 1, HundredThousands = 2, Thousands = 3, none = 4 };

	const MoneyFactor BestSuitedMoneyFactor(vector<float> vals) {

		float average = 0;
		for (const float& val : vals) { average += val; }
		average /= (float)vals.size();
		
		return HigestMoneyFactor(average);
	}

	const MoneyFactor HigestMoneyFactor(float val) {
		if (val > MoneyFactor_ToNum(Billions)) { return Billions; }
		else if (val > MoneyFactor_ToNum(Millions)) { return Millions; }
		else if (val > MoneyFactor_ToNum(HundredThousands)) { return HundredThousands; }
		else if (val > MoneyFactor_ToNum(Thousands)) { return Thousands; }
		else { return none; }
	}

	const inline string MoneyFactor_ToString(MoneyFactor moneyFactor) {
		static const string factorsNames[] = { "Billion", "Million", "Hundred Thousand", "Thousand", "" };
		return factorsNames[(int)moneyFactor];
	}

	const inline float MoneyFactor_ToNum(MoneyFactor moneyFactor) {
		static const float factorsNum[] = { 1000000000.0f, 1000000.0f, 100000.0f, 1000.0f, 1.0f };
		return factorsNum[(int)moneyFactor];
	}
};

struct FQDate {
	static const uint8_t BYTESIZE = sizeof(uint8_t) + sizeof(Quater);

	//year stored as 1900 + year
	uint8_t year;
	Quater quater;

	const int ActualYear() { return 1900 + year; }

	void Print() { std::cout << "Q" << (int)quater << " " << (int)year + 1900 << "\n\n"; }

	string ToString() {
		std::stringstream ss;
		ss << "Q" << (int)quater << " " << (int)year + 1900;
		return ss.str(); 
	}

	FQDate() : year(0), quater(None){}

	FQDate(int _actualYear) : year(_actualYear - 1900), quater(None) {}
	FQDate(int _actualYear, Quater _quater) : year(_actualYear - 1900), quater(_quater){}

	FQDate(uint8_t _year) : year(_year), quater(None) {}
	FQDate(uint8_t _year, Quater _quater) : year(_year), quater(_quater) {}

	FQDate(char* binData):FQDate() {
		memcpy(&year, &binData[0], sizeof(uint8_t));
		memcpy(&quater, &binData[sizeof(uint8_t)], sizeof(Quater));
	}

	unique_ptr<char[]> ToBinary() {
		unique_ptr<char[]> binaryData = unique_ptr<char[]>(new char[BYTESIZE]);
		memcpy(&binaryData[0], &year, sizeof(uint8_t));
		memcpy(&binaryData[sizeof(uint8_t)], &quater, sizeof(Quater));
		return binaryData;
	}
};

struct FQ_Statement {
	FQDate date;

	FQ_Statement(FQDate _date) :date(_date){}

	virtual FQDate GetDate() { return date; }
	virtual string ToString() { return ""; }
	virtual void PrintSummary() { return; }
};


struct IncomeStatement_FQ : MoneyFactoring, public FQ_Statement {
	static const uint8_t BYTESIZE = FQDate::BYTESIZE + sizeof(float) * 3;

	float Revenue;
	float OperatingExpense;
	float NetIncome;

	const float NetProfitMargin() { return NetIncome / Revenue; }
	const float EarningsPerShare(uint64_t numShares) { return NetIncome / (double)numShares; }

	IncomeStatement_FQ():FQ_Statement(FQDate()), Revenue(0), OperatingExpense(0), NetIncome(0) {}

	IncomeStatement_FQ(FQDate _date, float _revenue, float _operatingExpense, float _netIncome)
		:FQ_Statement(_date), Revenue(_revenue), OperatingExpense(_operatingExpense), NetIncome(_netIncome) {}

	IncomeStatement_FQ(char* binData):IncomeStatement_FQ() {
		date = FQDate(&binData[0]);
		memcpy( &Revenue, &binData[2 + 0 * sizeof(float)], sizeof(float));
		memcpy( &OperatingExpense, &binData[2 + 1 * sizeof(float)], sizeof(float));
		memcpy( &NetIncome, &binData[2 + 2 * sizeof(float)], sizeof(float));
	}

	void PrintSummary() override {
		std::cout << "Income Statement:\n" << "- " << date.ToString() << " -\n";
		std::cout << "Revenue: " << Revenue << "\n";
		std::cout << "Operating - Expense: " << OperatingExpense << "\n";
		std::cout << "Net-Income: " << NetIncome << "\n\n";
	}

	string ToString() override {
		std::stringstream ss;
		ss << date.ToString() << " Revenue: " << Revenue << " Operating-Expense: " << OperatingExpense << " Net-Income: " << NetIncome;
		return ss.str();
	}

	unique_ptr<char[]> ToBinary() {
		unique_ptr<char[]> binaryData = unique_ptr<char[]>(new char[BYTESIZE]);
		memcpy(&binaryData[0], &date.ToBinary()[0], FQDate::BYTESIZE);
		memcpy(&binaryData[2 + 0 * sizeof(float)], &Revenue, sizeof(float));
		memcpy(&binaryData[2 + 1 * sizeof(float)], &OperatingExpense, sizeof(float));
		memcpy(&binaryData[2 + 2 * sizeof(float)], &NetIncome, sizeof(float));
		return binaryData;
	}
};

struct BalanceSheet_FQ : MoneyFactoring, public FQ_Statement {
	static const uint8_t BYTESIZE = FQDate::BYTESIZE + sizeof(float) * 5;

	float Cash;
	float SharesOutstanding;

	float TotalAssets;
	float TotalLiabilites;
	float TotalEquity;

	const float Equity() { return TotalAssets - TotalLiabilites; }
	
	void PrintSummary() override {
		std::cout << "Balance Sheet:\n" << "- " << date.ToString() << " -\n";
	}

	string ToString() override {
		std::stringstream ss;
		ss << date.ToString();
		return ss.str();
	}

	unique_ptr<char[]> ToBinary() {
		unique_ptr<char[]> binaryData = unique_ptr<char[]>(new char[BYTESIZE]);
		memcpy(&binaryData[0], &date.ToBinary()[0], FQDate::BYTESIZE);
		//memcpy(&binaryData[2 + 0 * sizeof(float)], &Revenue, sizeof(float));
		return binaryData;
	}
};

struct CashFlow_FQ : MoneyFactoring, public FQ_Statement {
	static const uint8_t BYTESIZE = 0;

	float NetIncome;

	float Cash_FromOperations;
	float Cash_FromInvesting;
	float Cash_FromFinancing;

	float FreeCashFlow;

	const float NetChangeInCash() { return Cash_FromOperations + Cash_FromInvesting + Cash_FromFinancing; }

	void PrintSummary() override {
		std::cout << "Cash Flow:\n" << "- " << date.ToString() << " -\n";
	}

	string ToString() override {
		std::stringstream ss;
		ss << date.ToString();
		return ss.str();
	}

	unique_ptr<char[]> ToBinary() {
		unique_ptr<char[]> binaryData = unique_ptr<char[]>(new char[BYTESIZE]);
		memcpy(&binaryData[0], &date.ToBinary()[0], FQDate::BYTESIZE);
		//memcpy(&binaryData[2 + 0 * sizeof(float)], &Revenue, sizeof(float));
		return binaryData;
	}
};

struct FinanceQuaterSummary {

	IncomeStatement_FQ incomeStatement;
	BalanceSheet_FQ balanceSheet;
	CashFlow_FQ cashFlow;

	const float ReturnOnAssets() { return 0.0f; }//return netIncome / TotalAssets; 	//these dont add up on google
	const float ReturnOnEquity() { return 0.0f; }//return netIncome / (TotalAssets - Equity()); 


};


struct FinanceHistory_Year {


};


