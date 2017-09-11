#ifndef LRULENEIGHBORS_C
#define LRULENEIGHBORS_C

#include "LRuleNeighbors.h"
#include <stdbool.h>
#include <stdlib.h>

bool resetNeighbs(bool*** neighbs, unsigned int numRules, unsigned int numModules);
bool resetNeighbs(bool*** neighbs, unsigned int numRules, unsigned int numModules)
{
  bool** tmp;

  if(!(tmp= calloc(numRules, sizeof(bool*) )))
    return false;
  
  for(unsigned int i=0; i < numRules; i++)
    tmp[i] = NULL; 
    
  for(unsigned int i=0; i < numRules; i++)
    if(!(tmp[i] = calloc(numModules, sizeof(bool))))
    {
      for(unsigned int i=0; i < numRules; i++)
        if( tmp[i] != NULL ) free(tmp[i]);
      return free(tmp), false;
    }

  *neighbs = tmp;
  return true;
}

bool lrn_reset(LRuleNeigbors** lerns, unsigned int numRules, unsigned int numModules)
{
  LRuleNeigbors* tmp; 

  if(*lerns != NULL) lrn_free(*lerns, numRules);

  if( !(tmp = malloc(sizeof(*tmp)) ) ) return false;

  if( !(resetNeighbs(&tmp->lerLCNeighbors, numRules, numModules) ) )
    return lrn_free(tmp, numRules), false;

  if( !(resetNeighbs(&tmp->lerRCNeighbors, numRules, numModules) ) )
    return lrn_free(tmp, numRules), false;

  *lerns = tmp;
  return true;
}

void lrn_free(LRuleNeigbors* lerns, unsigned int numRules)
{
  if( lerns == NULL ) return;

  for(unsigned int i=0; lerns->lerLCNeighbors && i < numRules; i++)
    if( lerns->lerLCNeighbors[i] != NULL ) free(lerns->lerLCNeighbors[i]);
  free(lerns->lerLCNeighbors);

  for(unsigned int i=0; lerns->lerRCNeighbors && i < numRules; i++)
    if( lerns->lerRCNeighbors[i] != NULL ) free(lerns->lerRCNeighbors[i]);
  free(lerns->lerRCNeighbors);

  free(lerns);
}

#endif
