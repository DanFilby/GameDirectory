#pragma once
#include "Common.h"

//share
//quater
//market cap

struct MoneyFactoring {
	enum MoneyFactor { Billions = 0, Millions = 1, HundredThousands = 2, Thousands = 3, none = 4 };

	const MoneyFactor BestSuitedMoneyFactor(vector<float> vals) {

		float average = 0;
		for (const float& val : vals) { average += val; }
		average /= (float)vals.size();
		
		MoneyFactor highestFactor = HigestMoneyFactor(average);
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


struct FinancialQuater : MoneyFactoring {
	//date


};


