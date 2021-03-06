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
	while (true)
	{
		std::cout << "service : "; cin >> s;
		service.setValue(s);
		cout << "tips -> " << system->evaluate() << endl;
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
	while (true)
	{
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
	fuzzy::IsTriangle<float> cold(-5, 0, 5);
	fuzzy::IsTriangle<float> good(0, 15, 30);
	fuzzy::IsTriangle<float> hot(15, 32.5, 50);

	fuzzy::IsTriangle<float> cool(0, 5, 10);
	fuzzy::IsTriangle<float> stable(10, 15, 20);
	fuzzy::IsTriangle<float> heat(20, 25, 30);

	//values
	interpret::ValueModel<float> temperature(0);

	interpret::Expression<float> *r =
		f.newAgg(
			f.newAgg(
				f.newThen(
					f.newIs(&cold, &temperature),
					f.newIs(&heat, &temperature)
				),
				f.newThen(
					f.newIs(&good, &temperature),
					f.newIs(&stable, &temperature)
				)
			),
			f.newThen(
				f.newIs(&hot, &temperature),
				f.newIs(&cool, &temperature)
			)
		);

	//defuzzification
	interpret::Expression<float> *system = f.newDefuzz(&temperature, r, -5, 50, 1);

	//apply input
	float s;
	while (true)
	{
		std::cout << "Temperature : "; cin >> s;
		temperature.setValue(s);
		cout << "? -> " << system->evaluate() << endl;
	}
}

int main()
{
	testBinaryExpression();
	testUnaryExpression();
	//testFactory();
//	testSugenoDefuzz();
	testTempController();
	return 0;
}

