#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "HLSystem.h"
#include "ILSystem.h"
#include "Errors.h"

#include <stdbool.h>

ILSystem* check_hlsystem(HLSystem* hls, LSysErrors* errs);

#endif
