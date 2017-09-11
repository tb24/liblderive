#ifndef LRULENEIGHBORS_H
#define LRULENEIGHBORS_H

#include <stdbool.h>
// Stores whether the module type was a neighboring 
// module during derivation of the given rule. 
// By neighboring, I mean that the module type was one 
// of those immediately next to the last module in the 
// context whenever the rule matched.
typedef struct LRuleNeigbors
{
  bool** lerLCNeighbors;
  bool** lerRCNeighbors;
} LRuleNeigbors;

void lrn_free(LRuleNeigbors* lerns, unsigned int numRules);
bool lrn_reset(LRuleNeigbors** lerns, unsigned int numRules, unsigned int numModules);

#endif
