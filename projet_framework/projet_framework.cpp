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
	fuzzy::NotMinus1<int> opNot;
	fuzzy::AndMin<int> opAnd;
	fuzzy::OrMax<int> opOr;
	fuzzy::ThenMin<int> opThen;
	fuzzy::AggMax<int> opAgg;
	fuzzy::IsTriangle<int> opIsT;
	fuzzy::CogDefuzz<int> opDefuzz;

	//fuzzy expession factory
	interpret::FuzzyFactory<int> f(&opAnd, &opOr, &opAgg, &opDefuzz, &opNot, &opThen, &opIsT);

	//membership function
	fuzzy::IsTriangle<int> poor(-5, 0, 5);
	fuzzy::IsTriangle<int> good(0, 5, 10);
	fuzzy::IsTriangle<int> excellent(5, 10, 15);
	fuzzy::IsTriangle<int> cheap(0, 5, 10);
	fuzzy::IsTriangle<int> average(10, 15, 20);
	fuzzy::IsTriangle<int> generous(20, 25, 30);

	//values
	interpret::ValueModel<int> service(0);
	interpret::ValueModel<int> food(0);
	interpret::ValueModel<int> tips(0);

	interpret::Expression<int> *r =
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
	interpret::Expression<int> *system = f.newDefuzz(&tips, r, 0, 25, 1);

	//apply input
	float s;
	while (true)
	{
		std::cout << "service : "; cin >> s;
		service.setValue(s);
		cout << "tips -> " << system->evaluate() << endl;
	}
}

int main()
{
	testBinaryExpression();
	testUnaryExpression();
	testFactory();

	return 0;
}

