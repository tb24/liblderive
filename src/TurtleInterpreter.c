#ifndef TURTLE_INTERP_C
#define TURTLE_INTERP_C

#include "Colors.h"
#include "Vector.h"
#include "LVbo.h"
#include "Defines.h"
#include "LString.h"
#include "LSModules.h"
#include "LTurtle.h"

void appendCylinder(float length, LTurtle* turt, LVbo* lvbo);
//void appendCylinder(float length, LTurtle* turt, LVbo* lvbo, bool appending);
void appendLine(float lenth, LTurtle* turt, LVbo* lvbo);
void addCylindricSection(LTurtle* turt, LVbo* lvbo);

void endTri(LTurtle* turt, LVbo* lvbo, unsigned int triPointCount);
void addTriPoint(LTurtle* turt, LVbo* lvbo);

bool renderLString(LString* lstr, IModType* drawTypes, LVbo* lvbo, LSysDrawMode drawMode)
{
  LStringPosition lsp = lsp_head(lstr);

  LTurtle* turt = lturtle_make();
  unsigned int numTris = 0;
  bool isNumTrisOdd = false;
  bool appending   = false;
  bool gotFirstTP  = false;
  bool gotSecondTP = false;

  Vertex firstTP  = { {0,0,0,0}, {0,0,0}, {0,0,0} };
  Vertex secondTP = { {0,0,0,0}, {0,0,0}, {0,0,0} };

  while(!lsp_isAtEnd(lsp))
  {
    switch(drawTypes[lsp_mt(lsp)])
    {
      case mtForward   :
        if(drawMode ==     lsys_draw_mode_linear) appendLine(lsm_paramI(lsp, 0), turt, lvbo); 
        if(drawMode == lsys_draw_mode_volumetric) appendCylinder(lsm_paramI(lsp,0), turt, lvbo);
        break;
        break;
      case mtTriStart  : 
        break;
      case mtTriEnd    :
        // cap off the last strip; if there was one
        if(appending)
        {
            GLushort indx = -1;
            addIndexes(lvbo, &indx, 1);
        }
        appending = false;
        gotFirstTP  = false;
        gotSecondTP  = false;
        break;
      case mtTriPoint  :
        if(!gotFirstTP)
        {
            gotFirstTP = true;
            firstTP.n = turt->currentState->up;
            firstTP.p = turt->currentState->pos;
            firstTP.c = hsva2rgba ( &turt->currentState->color );
            break;
        }
        if(!gotSecondTP)
        {
            gotSecondTP = true;
            secondTP.n = turt->currentState->up;
            secondTP.p = turt->currentState->pos;
            secondTP.c = hsva2rgba ( &turt->currentState->color );
            break;
        }
        if(!appending)
        {
            numTris++;
            appending = true;
            isNumTrisOdd = true;
            // Add the first triangle          
            GLushort indxs[3] = { 0, 1, 2 };
            Vertex verts[3];
            verts[2].n = turt->currentState->up;
            verts[2].p = turt->currentState->pos;
            verts[2].c = hsva2rgba ( &turt->currentState->color );
            verts[1] = secondTP;
            verts[0] = firstTP;
            addIndexes(lvbo, indxs, 3);
            addVertexs(lvbo, verts, 3);
            break;
        }
        // Add the next triangle point to the list
        numTris++;
        GLushort indx = 0;
        Vertex vert;
        vert.n = turt->currentState->up;
        vert.p = turt->currentState->pos;
        vert.c = hsva2rgba ( &turt->currentState->color );
        addIndexes(lvbo, &indx, 1);
        addVertexs(lvbo, &vert, 1);
        isNumTrisOdd = !isNumTrisOdd;
        break;
      case mtPush      : tPush(turt); break;
      case mtPop       : tPop(turt) ; break;
      case mtUnPush    : tUnPush(turt); break;
      case mtRePush    : tRePush(turt); break;
      case mtTiptoe    : tForward   ( turt, lsm_paramI(lsp, 0) ); break;
      case mtCircle    : break; 
      case mtSphere    : break;
      case mtRollLeft  : tRollLeft  ( turt, lsm_paramI(lsp,0) ); break;
      case mtRollRight : tRollRight ( turt, lsm_paramI(lsp,0) ); break;
      case mtLeft      : tLeft      ( turt, lsm_paramI(lsp,0) ); break;
      case mtRight     : tRight     ( turt, lsm_paramI(lsp,0) ); break;
      case mtUp        : tUp        ( turt, lsm_paramI(lsp,0) ); break;
      case mtDown      : tDown      ( turt, lsm_paramI(lsp,0) ); break;
      case mtAboutFace : tLeft      ( turt, 180              ); break;
      case mtWidth     : tSetWidth  ( turt, lsm_paramI(lsp,0) ); break;
      case mtColor     : 
        tSetColor( turt, 
            colorHSVA(lsm_paramI(lsp,0), lsm_paramI(lsp,1), lsm_paramI(lsp,2), 1) 
        ); 
        break;
      case mtColorHue  : tShiftHue(turt, lsm_paramI(lsp,0)); break;
      case mtColorSat  : tShiftSat(turt, lsm_paramI(lsp,0)); break;
      case mtColorVal  : tShiftVal(turt, lsm_paramI(lsp,0)); break;
      default          : break;
    } 
    lsp = lsp_moveR(lsp);
  }
  DBG( fprintf(stderr, " done.\n"); )
    //VERB( printBufferContent(lvbo, stderr); )

    lturtle_free(turt);
  return true;
}

void appendLine(float length, LTurtle* turt, LVbo* lvbo)
{
  Vertex verts[2];

  verts[0].p = turt->currentState->pos;
  verts[0].n = turt->currentState->up;
  verts[0].c = hsva2rgba ( &turt->currentState->color );

  tForward(turt, length);

  verts[1].p = turt->currentState->pos;
  verts[1].n = turt->currentState->up;
  verts[1].c = hsva2rgba ( &turt->currentState->color );

  addLine(lvbo, verts, 2);

}

void addTriPoint(LTurtle* turt, LVbo* lvbo)
{
  GLushort indx = 0;
  Vertex v;
  v.n = turt->currentState->up;
  v.p = turt->currentState->pos;
  v.c = hsva2rgba ( &turt->currentState->color );
  addIndexes(lvbo, &indx, 1);
  addVertexs(lvbo, &v, 1); 
  //fprintf(stderr,"n: %f %f %f\n",v.n.x, v.n.y, v.n.z);
}


// TODO actually fix the bugs here
void endTri(LTurtle* turt, LVbo* lvbo, unsigned int triPointCount)
{
  GLushort indx[2] = { -1, 0 };
  GLushort indxx = 0;
 
  //VERB( fprintf(stderr, "et(%04d)", triPointCount); )
  //fprintf(stderr, "et(%04d) ", triPointCount); 

  // we need to cap off the triangle strip with degenerate triangles
  // it depends on how many triangles we have
  // also prepare it to be joined with the next strinp 
  if ( triPointCount == 0 )    { return; }
  if ( triPointCount == 1 )    { 
      //GLushort indx = 0;
      Vertex v;
      v.n = turt->currentState->up;
      v.p = turt->currentState->pos;
      v.c = hsva2rgba ( &turt->currentState->color );
      //  addIndexes(lvbo, &indx, 1);
      addVertexs(lvbo, &v, 1); 
      return; 
  }
  if ( triPointCount == 2 )    { 
      Vertex v;
      v.n = turt->currentState->up;
      v.p = turt->currentState->pos;
      v.c = hsva2rgba ( &turt->currentState->color );
      addIndexes(lvbo, &indxx, 1);
//      addVertexs(lvbo, &v, 1); 
      return; 
  }
  if ( triPointCount % 2 == 1 ){ 
      addIndexes(lvbo, indx, 1);
      return; 
  }
  if ( triPointCount % 2 == 0 ){ 
      addIndexes(lvbo, indx, 2); 
  //fprintf(stderr, "bet(%04d) ", triPointCount); 
      return; 
  }
}

void appendCylinder(float length, LTurtle* turt, LVbo* lvbo)
{
  
  GLushort  inds[14];
  
  addCylindricSection(turt, lvbo);
  
  inds[ 0] = - 5;
  inds[ 1] = - 5;
  inds[ 2] = + 0;
  inds[ 3] = - 4;
  inds[ 4] = + 1;
  inds[ 5] = - 3;
  inds[ 6] = + 2;
  inds[ 7] = - 2;
  inds[ 8] = + 3;
  inds[ 9] = - 1;
  inds[10] = + 4;
  inds[11] = - 5;
  inds[12] = + 0;
  inds[13] = + 0;
  
  addIndexes(lvbo, inds, 14);
  
  tForward(turt, length);
  
  addCylindricSection(turt, lvbo);
  
}

// Uses the orientation of the given turtle to define
// a pentagon that approxomates a section of a cylinder
void addCylindricSection(LTurtle* turt, LVbo* lvbo)
{
  
  Vertex verts[5];
  
  // Use the orientation of the given turtle
  
  for(int i=0; i<5; i++){
    vRotate(&turt->currentState->head, i*72, &turt->currentState->left, &verts[i].n);
    vNorm(&verts[i].n);
    vScale(&verts[i].n, turt->currentState->width, &verts[i].p);
    vAdd(&turt->currentState->pos, &verts[i].p, &verts[i].p);
    verts[i].c = hsva2rgba( &turt->currentState->color);
  }
  
  addVertexs(lvbo, verts, 5);
  
}


#endif
