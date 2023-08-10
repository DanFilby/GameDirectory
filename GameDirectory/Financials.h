#pragma once
#include "Common.h"

//share
//quater
//market cap
//balance sheet 
//cash flow

enum Quater : uint8_t { None = 0, Q1 = 1, Q2 = 2, Q3 = 3, Q4 = 4 };

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


struct FinancialQuater : MoneyFactoring {


	static const uint8_t BYTESIZE = sizeof(float) * 3 + FQDate::BYTESIZE;

	FQDate date;

	float Revenue;
	float OperatingExpense;
	float NetIncome;

	const float NetProfitMargin() { return NetIncome / Revenue; }
	const float EarningsPerShare(uint64_t numShares) { return NetIncome / (double)numShares; }

	FinancialQuater():date(), Revenue(0), OperatingExpense(0), NetIncome(0) {}



};


