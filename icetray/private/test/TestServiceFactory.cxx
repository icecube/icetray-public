// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include "TestServiceFactory.h"

I3_SERVICE_FACTORY(TestServiceFactory);

TestServiceFactory* TestServiceFactory::instance = 0;
double TestServiceFactory::doubleParam;
std::string TestServiceFactory::stringParam;
bool TestServiceFactory::boolParam;
float TestServiceFactory::floatParam;
int TestServiceFactory::intParam;
long TestServiceFactory::longParam;
