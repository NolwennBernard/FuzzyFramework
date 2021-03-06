// projet_framework.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include "interpret\Expression.h"
#include "interpret\ValueModel.h"
#include "interpret\BinaryExpression.h"
#include "interpret\BinaryExpressionModel.h"
#include "interpret\UnaryExpression.h"
#include "interpret\UnaryExpressionModel.h"
#include "interpret\BinaryShadowExpression.h"
#include "interpret\FuzzyFactory.h"
#include "fuzzy\AndMin.h"
#include "fuzzy\AndMult.h"
#include "fuzzy\ThenMin.h"
#include "fuzzy\ThenMult.h"
#include "fuzzy\OrMax.h"
#include "fuzzy\OrPlus.h"
#include "fuzzy\AggMax.h"
#include "fuzzy\AggPlus.h"
#include "fuzzy\NotMinus1.h"
#include "fuzzy\CogDefuzz.h"
#include "fuzzy\IsTriangle.h"
#include "fuzzy\IsTrapez.h"
#include "fuzzy\IsTrapez1.h"
#include "fuzzy\IsTrapez2.h"
#include<iostream>
#include <assert.h>

void testBinaryExpression() {

	interpret::ValueModel<int> l(1);
	interpret::ValueModel<int> r(3);

	fuzzy::AndMin<int> andMin;
	interpret::ValueModel<int> valueAndMin = andMin.evaluate(&l, &r);
	assert(valueAndMin.evaluate() == 1);

	fuzzy::AndMult<int> andMult;
	interpret::ValueModel<int> valueAndMult = andMult.evaluate(&l, &r);
	assert(valueAndMult.evaluate() == 3);

	fuzzy::ThenMin<int> thenMin;
	interpret::ValueModel<int> valueThenMin = thenMin.evaluate(&l, &r);
	assert(valueThenMin.evaluate() == 1);

	fuzzy::ThenMult<int> thenMult;
	interpret::ValueModel<int> valueThenMult = thenMult.evaluate(&l, &r);
	assert(valueThenMult.evaluate() == 3);

	fuzzy::OrMax<int> orMax;
	interpret::ValueModel<int> valueOrMax = orMax.evaluate(&l, &r);
	assert(valueOrMax.evaluate() == 3);

	fuzzy::OrPlus<int> orPlus;
	interpret::ValueModel<int> valueOrPlus = orPlus.evaluate(&l, &r);
	assert(valueOrPlus.evaluate() == 4);

	fuzzy::AggMax<int> aggMax;
	interpret::ValueModel<int> valueAggMax = aggMax.evaluate(&l, &r);
	assert(valueAggMax.evaluate() == 3);

	fuzzy::AggPlus<int> aggPlus;
	interpret::ValueModel<int> valueAggPlus = aggPlus.evaluate(&l, &r);
	assert(valueAggPlus.evaluate() == 4);

	interpret::BinaryExpressionModel<int> binExp(&l, &r, &andMin);
	interpret::ValueModel<int> valueBinExp = binExp.evaluate();
	assert(valueBinExp.evaluate() == 1);

}

void testUnaryExpression() {
	interpret::ValueModel<int> exp(2);

	fuzzy::NotMinus1<int> notMinus1;
	interpret::ValueModel<int> valueNotMinus1 = notMinus1.evaluate(&exp);
	assert(valueNotMinus1.evaluate() == -1);

	fuzzy::IsTriangle<int> isTriangle;
	interpret::ValueModel<int> valueIsTriangle = isTriangle.evaluate(&exp);
	assert(valueIsTriangle.evaluate() == 0);

	fuzzy::IsTrapez<int> isTrapez;
	interpret::ValueModel<int> valueIsTrapez = isTrapez.evaluate(&exp);
	assert(valueIsTrapez.evaluate() == 1);

	fuzzy::IsTrapez1<int> isTrapez1;
	interpret::ValueModel<int> valueIsTrapez1 = isTrapez1.evaluate(&exp);
	assert(valueIsTrapez1.evaluate() == 0);

	fuzzy::IsTrapez2<int> isTrapez2;
	interpret::ValueModel<int> valueIsTrapez2 = isTrapez2.evaluate(&exp);
	assert(valueIsTrapez2.evaluate() == 1);
}

void testFactory() {
	fuzzy::NotMinus1<float> opNot;
	fuzzy::AndMin<float> opAnd;
	fuzzy::OrMax<float> opOr;
	fuzzy::ThenMin<float> opThen;
	fuzzy::AggMax<float> opAgg;
	fuzzy::IsTriangle<float> opIsT;
	fuzzy::CogDefuzz<float> opDefuzz;

	//fuzzy expession factory
	interpret::FuzzyFactory<float> f(&opAnd, &opOr, &opAgg, &opDefuzz, &opNot, &opThen, &opIsT);

	//membership function
	fuzzy::IsTriangle<float> poor(-5, 0, 5);
	fuzzy::IsTriangle<float> good(0, 5, 10);
	fuzzy::IsTriangle<float> excellent(5, 10, 15);

	fuzzy::IsTriangle<float> cheap(0, 5, 10);
	fuzzy::IsTriangle<float> average(10, 15, 20);
	fuzzy::IsTriangle<float> generous(20, 25, 30);

	//values
	interpret::ValueModel<float> service(0);
	interpret::ValueModel<float> food(0);
	interpret::ValueModel<float> tips(0);

	interpret::Expression<float> *r =
		f.newAgg(
			f.newAgg(
				f.newThen(
					f.newIs(&poor, &service),
					f.newIs(&cheap, &tips)
				),
				f.newThen(
					f.newIs(&good, &service),
					f.newIs(&average, &tips)
				)
			),
			f.newThen(
				f.newIs(&excellent, &service),
				f.newIs(&generous, &tips)
			)
		);

	//defuzzification
	interpret::Expression<float> *system = f.newDefuzz(&tips, r, 0, 25, 1);

	//apply input
	float s;
	int i = 0;
	while (i < 2) {
		std::cout << "service : "; cin >> s;
		service.setValue(s);
		cout << "tips -> " << system->evaluate() << endl;
		i++;
	}
}

void testSugenoDefuzz()
{
	//operators
	fuzzy::NotMinus1<float>    opNot;
	fuzzy::AndMin<float>       opAnd;
	fuzzy::OrMax<float>        opOr;
	fuzzy::SugenoThen<float>   opThen;
	fuzzy::AggMax<float>       opAgg;
	fuzzy::SugenoDefuzz<float> opSugDefuzz;

	std::vector<float> coef;
	coef.push_back(1);
	coef.push_back(1);
	coef.push_back(1);
	fuzzy::SugenoConclusion<float> opConclusion(&coef);

	//fuzzy expression factory
	interpret::FuzzyFactory<float> f(&opNot, &opAnd, &opOr, &opThen, &opAgg, &opSugDefuzz, &opConclusion);

	//membership function
	// service
	fuzzy::IsTriangle<float> poor(-5, 0, 5);
	fuzzy::IsTriangle<float> good(0, 5, 10);
	fuzzy::IsTriangle<float> excellent(5, 10, 15);
	// food
	fuzzy::IsTriangle<float> rancid(-5, 0, 5);
	fuzzy::IsTriangle<float> delicious(5, 10, 15);
	//values
	interpret::ValueModel<float> service(0);
	interpret::ValueModel<float> food(0);

	//output : ces variables vont stocker le résultat de SujenoConclusion (zi)
	std::vector<interpret::Expression<float>*> SC_service_food;
	SC_service_food.push_back(&service);
	SC_service_food.push_back(&food);

	std::vector<interpret::Expression<float>*> SC_service;
	SC_service.push_back(&service);

	//rules
	std::vector<interpret::Expression<float>*> rules;

	rules.push_back(
		f.newThen(
			f.newOr(
				f.newIs(&poor, &service),
				f.newIs(&rancid, &food)
			),
			f.newNConclusion(&SC_service_food)
		));

	rules.push_back(
		f.newThen(
			f.newIs(&good, &service),
			f.newNConclusion(&SC_service)
		));

	rules.push_back(
		f.newThen(
			f.newOr(
				f.newIs(&excellent, &service),
				f.newIs(&delicious, &food)
			),
			f.newNConclusion(&SC_service_food)
		));

	//defuzzification
	interpret::Expression<float> *system = f.newSugeno(&rules);

	//apply input
	float s, foo;
	while (true) {
		std::cout << "service : ";
		std::cin >> s;
		service.setValue(s);
		std::cout << "food : ";
		std::cin >> foo;
		food.setValue(foo);
		std::cout << "tips -> " << system->evaluate() << std::endl;
	}
}


void testTempController() {

	fuzzy::NotMinus1<float> opNot;
	fuzzy::AndMin<float> opAnd;
	fuzzy::OrMax<float> opOr;
	fuzzy::ThenMin<float> opThen;
	fuzzy::AggMax<float> opAgg;
	fuzzy::IsTriangle<float> opIsT;
	fuzzy::CogDefuzz<float> opDefuzz;

	//fuzzy expession factory
	interpret::FuzzyFactory<float> f(&opAnd, &opOr, &opAgg, &opDefuzz, &opNot, &opThen, &opIsT);

	//membership function
	//temperature
	fuzzy::IsTrapez<float> cold(-15, -10, 10, 15);
	fuzzy::IsTriangle<float> good(0, 15, 30);
	fuzzy::IsTriangle<float> hot(15, 30, 45);

	//current temperature vs temperature
	fuzzy::IsTriangle<float> positif(0, 5, 10);
	fuzzy::IsTrapez1<float> negatif(-10, -5, 0);
	fuzzy::IsTriangle<float> neutral(-5, 0, 5);

	//action
	fuzzy::IsTriangle<float> cool(-10, -5, 0);
	fuzzy::IsTriangle<float> nothing(-5, 0, 5);
	fuzzy::IsTrapez2<float> heat(0, 5, 10);


	//values
	interpret::ValueModel<float> temperature(0);
	interpret::ValueModel<float> diffTemperature(0);
	interpret::ValueModel<float> action(0);


	interpret::Expression<float> *r =
		f.newAgg(
			f.newAgg(
				f.newThen(
					f.newOr(
						f.newAnd(
							f.newIs(&good, &temperature),
							f.newIs(&negatif, &diffTemperature)
						),
						f.newAnd(
							f.newIs(&cold, &temperature),
							f.newOr(
								f.newOr(
									f.newIs(&positif, &diffTemperature),
									f.newIs(&negatif, &diffTemperature)
								),
								f.newIs(&neutral, &diffTemperature)
							)
						)
					),
					f.newIs(&heat, &action)
				),
				f.newThen(
					f.newOr(
						f.newAnd(
							f.newIs(&good, &temperature),
							f.newIs(&positif, &diffTemperature)
						),
						f.newAnd(
							f.newIs(&hot, &temperature),
							f.newOr(
								f.newOr(
									f.newIs(&positif, &diffTemperature),
									f.newIs(&negatif, &diffTemperature)
								),
								f.newIs(&neutral, &diffTemperature)
							)
						)
					),
					f.newIs(&cool, &action)
				)
			),
			f.newThen(
				f.newAnd(
					f.newIs(&good, &temperature),
					f.newIs(&neutral, &diffTemperature)
				),
				f.newIs(&nothing, &action)
			)
		);
	/*
		f.newAgg(
			f.newAgg(
				f.newThen(
					f.newIs(&good,&temperature),
					f.newIs(&nothing,&action)
				),
				f.newThen(
					f.newIs(&cold,&temperature),
					f.newIs(&heat,&action)
				)
			),
			f.newThen(
				f.newIs(&hot,&temperature),
				f.newIs(&cool,&action)
			)
		);*/
		//defuzzification
	interpret::Expression<float> *system = f.newDefuzz(&action, r, -10, 10, 1);

	//apply input
	float s;
	float d;
	while (true) {
		std::cout << "Temperature : "; cin >> s;
		temperature.setValue(s);
		std::cout << "Difference de temperature : "; cin >> d;
		diffTemperature.setValue(d);
		cout << "action power-> " << system->evaluate() << endl;
	}
}

void SugenoTempController() {
	fuzzy::NotMinus1<float> opNot;
	fuzzy::AndMin<float> opAnd;
	fuzzy::OrMax<float> opOr;
	fuzzy::ThenMin<float> opThen;
	fuzzy::AggMax<float> opAgg;
	fuzzy::IsTriangle<float> opIsT;
	fuzzy::SugenoDefuzz<float> opSugDefuzz;

	std::vector<float> coef;
	coef.push_back(1);
	coef.push_back(1);
	coef.push_back(1);
	fuzzy::SugenoConclusion<float> opConclusion(&coef);

	//fuzzy expression factory
	interpret::FuzzyFactory<float> f(&opNot, &opAnd, &opOr, &opThen, &opAgg, &opSugDefuzz, &opConclusion);

	//membership function
	//temperature
	fuzzy::IsTrapez<float> cold(-15, -10, 10, 15);
	fuzzy::IsTriangle<float> good(0, 15, 30);
	fuzzy::IsTriangle<float> hot(15, 30, 45);

	//current temperature vs temperature
	fuzzy::IsTriangle<float> positif(0, 5, 10);
	fuzzy::IsTrapez1<float> negatif(-10, -5, 0);
	fuzzy::IsTriangle<float> neutral(-5, 0, 5);

	//action
	fuzzy::IsTriangle<float> cool(-10, -5, 0);
	fuzzy::IsTriangle<float> nothing(-5, 0, 5);
	fuzzy::IsTrapez2<float> heat(0, 5, 10);


	//values
	interpret::ValueModel<float> temperature(0);
	interpret::ValueModel<float> diffTemperature(0);
	interpret::ValueModel<float> action(0);

	//output : ces variables vont stocker le résultat de SujenoConclusion (zi)
	std::vector< interpret::Expression<float>*> SC_temperature_diffTemperature;
	SC_temperature_diffTemperature.push_back(&temperature);
	SC_temperature_diffTemperature.push_back(&diffTemperature);

	//rules
	std::vector< interpret::Expression<float>*> r;

	r.push_back(
		f.newThen(
			f.newOr(
				f.newAnd(
					f.newIs(&good, &temperature),
					f.newIs(&negatif, &diffTemperature)
				),
				f.newAnd(
					f.newIs(&cold, &temperature),
					f.newOr(
						f.newOr(
							f.newIs(&positif, &diffTemperature),
							f.newIs(&negatif, &diffTemperature)
						),
						f.newIs(&neutral, &diffTemperature)
					)
				)
			),
			f.newNConclusion(&SC_temperature_diffTemperature)));




	r.push_back(
		f.newThen(
			f.newOr(
				f.newAnd(
					f.newIs(&good, &temperature),
					f.newIs(&positif, &diffTemperature)
				),
				f.newAnd(
					f.newIs(&hot, &temperature),
					f.newOr(
						f.newOr(
							f.newIs(&positif, &diffTemperature),
							f.newIs(&negatif, &diffTemperature)
						),
						f.newIs(&neutral, &diffTemperature)
					)
				)
			),
			f.newNConclusion(&SC_temperature_diffTemperature)));

	r.push_back(
		f.newThen(
			f.newAnd(
				f.newIs(&good, &temperature),
				f.newIs(&neutral, &diffTemperature)
			),
			f.newNConclusion(&SC_temperature_diffTemperature)));
	//defuzzification
	interpret::Expression<float> *system = f.newSugeno(&r);

	//apply input
	float temp, difftemp;
	while (true) {
		std::cout << "temperature : ";
		std::cin >> temp;
		temperature.setValue(temp);
		std::cout << "Difference de temperature : ";
		std::cin >> difftemp;
		diffTemperature.setValue(difftemp);
		std::cout << "action power -> " << system->evaluate() << std::endl;
	}

}

int main()
{
	testBinaryExpression();
	testUnaryExpression();
	unsigned int test = 0;
	unsigned int select = 0;
	std::cout << "1: tips" << std::endl;
	std::cout << "2: AC" << std::endl;
	std::cout << "> ";
	std::cin >> test;

	if (test == 1) {

		std::cout << "1: Mamdani" << std::endl;
		std::cout << "2: Sugeno" << std::endl;
		std::cout << "> ";
		std::cin >> select;
		if (select == 1)
		{
			testFactory();
		}

		else
		{
			testSugenoDefuzz();
		}
		select = 0;
		test = 0;
	}

	else {
		std::cout << "1: Mamdani" << std::endl;
		std::cout << "2: Sugeno" << std::endl;
		std::cout << "> ";
		std::cin >> select;
		if (select == 1)
			testTempController();
		else
			SugenoTempController();

		select = 0;
		test = 0;
	}
	return 0;
}

