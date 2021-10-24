#include "TmpDeleterGrammar.h"

TmpDeleterGrammar::TmpDeleterGrammar() {
	registerToken(WorkDirectory, "WORK_DIRECTORY");
	registerToken(Interval, "INTERVAL");
}
