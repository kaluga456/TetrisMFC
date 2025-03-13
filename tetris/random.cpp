#include "stdafx.h"
#pragma hdrstop
#include "random.h"

void app::random_seed(std::default_random_engine& dre)
{
	dre.seed(static_cast<int>(std::random_device()()));
}