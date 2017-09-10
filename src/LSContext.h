#ifndef LSCONTEXT_H
#define LSCONTEXT_H

// Creating Strings
LString* lsc_empty  ();
void     lsc_free   (LString* str);
bool     lsc_append (LString* str, IModType mt, IParamId po, unsigned int tag);

// Data Access
IParamId     lsc_paramOffset(LStringPosition lsp);
unsigned int lsc_tag(LStringPosition lsp);

#endif
