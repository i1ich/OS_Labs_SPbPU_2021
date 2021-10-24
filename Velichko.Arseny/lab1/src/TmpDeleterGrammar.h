#pragma once

#include "Grammar.h"

class TmpDeleterGrammar : public Grammar {
public:
	TmpDeleterGrammar();

	enum Tokens {
		WorkDirectory,
		Interval,
	};
};
