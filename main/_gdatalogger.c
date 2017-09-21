/*****************************************************************************
// File: GDataLogger.c 
// Contents: Functions for recording data points in matlab format.
// Author: G. A. Borges.
*****************************************************************************/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "gqueue.h"
#include "gmatlabdatafile.h"
#include "gdatalogger.h"

#define QUEUE_DESTINATION_FILE  QUEUE_READ_GATE_0

/**********************************************************************************************
***** gDataLogger: Init
**********************************************************************************************/
int gDataLogger_Init(PGDATALOGGER pgDataLogger, char *filename, char *dirname)
{
	int i;

	// GQueue
	for (i=0;i<GDATALOGGER_MAXVARIABLES;++i){
		sprintf(pgDataLogger->Variables[i].VariableName," ");
		sprintf(pgDataLogger->Variables[i].VariableUnit," ");
		pgDataLogger->Variables[i].CircularQueue = NULL;
		pgDataLogger->Variables[i].GMatlabDataFileIndex = 0;
		pgDataLogger->Variables[i].Nc = 1;
		pgDataLogger->Variables[i].Nr = 1;
	}
	pgDataLogger->m_NumberOfVariables = 0;

	// GMatlabDataFile
	return(gMATLABDataFile_OpenWrite(&pgDataLogger->GMatlabDataFileConfig, filename, dirname));
}

/**********************************************************************************************
***** gDataLogger: DeclareVariable
**********************************************************************************************/
int gDataLogger_DeclareVariable(PGDATALOGGER pgDataLogger, char *varname, char *varunit, int nrows, int ncols, int queuesize)
{
	if(pgDataLogger->m_NumberOfVariables>=GDATALOGGER_MAXVARIABLES-1){
		return(FALSE);
	}
	queuesize = queuesize * nrows * ncols; // no caso, o tamanho da fila eh calculado em numero de elementos double.
	sprintf(pgDataLogger->Variables[pgDataLogger->m_NumberOfVariables].VariableName,"%s",varname);
	sprintf(pgDataLogger->Variables[pgDataLogger->m_NumberOfVariables].VariableUnit,"%s",varunit);
	pgDataLogger->Variables[pgDataLogger->m_NumberOfVariables].CircularQueue = (double *) malloc(queuesize * nrows * ncols * sizeof(double));
	if(pgDataLogger->Variables[pgDataLogger->m_NumberOfVariables].CircularQueue==NULL){
		return(FALSE);
	}
	pgDataLogger->Variables[pgDataLogger->m_NumberOfVariables].Nr = nrows;
	pgDataLogger->Variables[pgDataLogger->m_NumberOfVariables].Nc = ncols;
	gQUEUE_Init(&pgDataLogger->Variables[pgDataLogger->m_NumberOfVariables].CircularQueueControl, queuesize, 1); // Only one reading index 
	pgDataLogger->Variables[pgDataLogger->m_NumberOfVariables].GMatlabDataFileIndex = 0;
	
	++pgDataLogger->m_NumberOfVariables;

	return(TRUE);
}


/**********************************************************************************************
***** gDataLogger: InsertVariable
**********************************************************************************************/
int gDataLogger_InsertVariable(PGDATALOGGER pgDataLogger, char *varname, double *varvalue)
{
	int queueindex,varindex,n;
	long int nr,nc;
	int FlagQueueFull;
	
	// Procura pela variavel
	varindex = -1;
	for(n=0;n<pgDataLogger->m_NumberOfVariables;++n){
		if(strcmp(varname,pgDataLogger->Variables[n].VariableName)==0){
			varindex = n;
			break;
		}
	}
	if(varindex<0){
		return(FALSE);
	}

	// Insere o conteudo
	for(nr=0;nr<pgDataLogger->Variables[varindex].Nr;++nr){
		for(nc=0;nc<pgDataLogger->Variables[varindex].Nc;++nc){
			FlagQueueFull = gQUEUE_RequestWriteIndex(&pgDataLogger->Variables[varindex].CircularQueueControl, &queueindex);

			pgDataLogger->Variables[varindex].CircularQueue[queueindex] = varvalue[nr+(nc)*pgDataLogger->Variables[varindex].Nr];

			if(!FlagQueueFull){
				// The queue is full. Test if it is the reading head associated to matlab data files.
				if(pgDataLogger->Variables[varindex].CircularQueueControl.FlagFull[QUEUE_DESTINATION_FILE]==TRUE){
					// Empty in matlab data file.
					gDataLogger_MatfileUpdate(pgDataLogger);
					// Clear flag:
					pgDataLogger->Variables[varindex].CircularQueueControl.FlagFull[QUEUE_DESTINATION_FILE] = FALSE;
				}
			}
		}
	}

	return(TRUE);
}

/**********************************************************************************************
***** gDataLogger: MatfileUpdate
**********************************************************************************************/
int gDataLogger_MatfileUpdate(PGDATALOGGER pgDataLogger)
{
	int QueueIndex,nvar,datasize,i;
	double *v;
	char matvarname[70];
	
	// Esvaziar queues de todas as variaveis que vao para o arquivo:
	for(nvar=0;nvar<pgDataLogger->m_NumberOfVariables;++nvar){
		// Metodo otimizado, que salva em uma variavel MATLAB todas as ultimas leituras.
		if(!gQUEUE_GetNumberOfUnreadData(&pgDataLogger->Variables[nvar].CircularQueueControl, QUEUE_DESTINATION_FILE, &datasize)){
			return(FALSE);
		}
		if(datasize>0){ // Existem dados a serem salvos.
			v = (double *) malloc(datasize * sizeof(double));
			for(i=0;i<datasize;++i){
				gQUEUE_RequestReadIndex(&pgDataLogger->Variables[nvar].CircularQueueControl, QUEUE_DESTINATION_FILE, &QueueIndex);
				v[i] = pgDataLogger->Variables[nvar].CircularQueue[QueueIndex];
			}
			sprintf(matvarname,"%s_%li_%li",pgDataLogger->Variables[nvar].VariableName,pgDataLogger->Variables[nvar].GMatlabDataFileIndex,pgDataLogger->Variables[nvar].GMatlabDataFileIndex+datasize-1);
			gMATLABDataFile_SaveVector(&pgDataLogger->GMatlabDataFileConfig, matvarname, v, datasize);
			free(v);
			pgDataLogger->Variables[nvar].GMatlabDataFileIndex += datasize;
		}

		// Metodo antigo, que salva em uma variavel MATLAB apenas uma leitura.
/*		while(gQUEUE_RequestReadIndex(&pgDataLogger->Variables[nvar].CircularQueueControl, QUEUE_DESTINATION_FILE, &QueueIndex)){
			v = &pgDataLogger->Variables[nvar].CircularQueue[QueueIndex];
			datasize = 1;
			sprintf(matvarname,"%s_%i",pgDataLogger->Variables[nvar].VariableName,pgDataLogger->Variables[nvar].GMatlabDataFileIndex);
			gMATLABDataFile_SaveVector(&pgDataLogger->GMatlabDataFileConfig, matvarname, v, datasize);

			++pgDataLogger->Variables[nvar].GMatlabDataFileIndex;
		}
*/
	}

	return(TRUE);
}

/**********************************************************************************************
***** gDataLogger: Close
**********************************************************************************************/
int gDataLogger_Close(PGDATALOGGER pgDataLogger)
{
	int nvar,datasize;
	double v;
	char matvarname[70];

	// Garante salvar ultimos dados inseridos:
	gDataLogger_MatfileUpdate(pgDataLogger);

	// Cria variaveis que indicam o numero de cada variavel salva:
	for(nvar=0;nvar<pgDataLogger->m_NumberOfVariables;++nvar){
		v = (double)(pgDataLogger->Variables[nvar].GMatlabDataFileIndex);
		datasize = 1;
		sprintf(matvarname,"%s_size",pgDataLogger->Variables[nvar].VariableName);
		gMATLABDataFile_SaveVector(&pgDataLogger->GMatlabDataFileConfig, matvarname, &v, datasize);
	}

	// Salva o numero de linhas:
	for(nvar=0;nvar<pgDataLogger->m_NumberOfVariables;++nvar){
		v = (double)(pgDataLogger->Variables[nvar].Nr);
		datasize = 1;
		sprintf(matvarname,"%s_nr",pgDataLogger->Variables[nvar].VariableName);
		gMATLABDataFile_SaveVector(&pgDataLogger->GMatlabDataFileConfig, matvarname, &v, datasize);
	}

	// Salva o numero de colunas:
	for(nvar=0;nvar<pgDataLogger->m_NumberOfVariables;++nvar){
		v = (double)(pgDataLogger->Variables[nvar].Nc);
		datasize = 1;
		sprintf(matvarname,"%s_nc",pgDataLogger->Variables[nvar].VariableName);
		gMATLABDataFile_SaveVector(&pgDataLogger->GMatlabDataFileConfig, matvarname, &v, datasize);
	}

	// Fecha o arquivo.
	gMATLABDataFile_Close(&pgDataLogger->GMatlabDataFileConfig);

	// Deletar as filas: 
	for(nvar=0;nvar<pgDataLogger->m_NumberOfVariables;++nvar){
		sprintf(pgDataLogger->Variables[nvar].VariableName," ");
		sprintf(pgDataLogger->Variables[nvar].VariableUnit," ");
		free(pgDataLogger->Variables[nvar].CircularQueue);
		pgDataLogger->Variables[nvar].CircularQueue = NULL;
		pgDataLogger->Variables[nvar].GMatlabDataFileIndex = 0;
	}
	pgDataLogger->m_NumberOfVariables = 0;

	return(TRUE);
}

/****************************************************************
**** 
**** Scope
**** 
****************************************************************/
/*
int GDataLogger::ScopeCreate(CWnd *pIDCWindow, int nIDC, PGDATALOGGERSCOPEPARAMETERS pDataLoggerScopeParameters)
{
	// Verify whether there is available Scope:
	if (pgDataLogger->m_NumberOfScopes>=(GDATALOGGER_MAXSCOPES-1)){
		return(FALSE);
	}

	// nIDC, pWnd and pDC:
	pgDataLogger->Scopes[m_NumberOfScopes].nIDC = nIDC;
	pgDataLogger->Scopes[m_NumberOfScopes].pWnd = pIDCWindow;
	pgDataLogger->Scopes[m_NumberOfScopes].pDC  = pgDataLogger->Scopes[m_NumberOfScopes].pWnd->GetDC();
	pgDataLogger->Scopes[m_NumberOfScopes].NumberOfInsertedVariables = 0;

	// Parameters:
	memcpy(&pgDataLogger->Scopes[m_NumberOfScopes].Parameters,pDataLoggerScopeParameters,sizeof(GDATALOGGERSCOPEPARAMETERS));

	// Previously ploted? No
	for(int i=0;i<GDATALOGGER_MAXVARIABLES;++i){
		pgDataLogger->Scopes[m_NumberOfScopes].FirstPointPloted[i] = FALSE;
	}

	// Compute rectangles:
	CRect Rect;
	TEXTMETRIC TextMetric;

	pgDataLogger->Scopes[m_NumberOfScopes].pWnd->GetWindowRect(&Rect);
	pgDataLogger->Scopes[m_NumberOfScopes].Parameters.RectDCPix.left   = 0;
	pgDataLogger->Scopes[m_NumberOfScopes].Parameters.RectDCPix.top    = 0;
	pgDataLogger->Scopes[m_NumberOfScopes].Parameters.RectDCPix.right  = Rect.right - Rect.left;
	pgDataLogger->Scopes[m_NumberOfScopes].Parameters.RectDCPix.bottom = Rect.bottom - Rect.top;

	pgDataLogger->Scopes[m_NumberOfScopes].pDC->GetTextMetrics(&TextMetric); 
	pgDataLogger->Scopes[m_NumberOfScopes].Parameters.RectPlotPix.left   = 
		+ pgDataLogger->Scopes[m_NumberOfScopes].Parameters.RectDCPix.left 
		+ 2*TextMetric.tmHeight // Numeros dos eixos
		+ 1*TextMetric.tmHeight + 2; // Label
	pgDataLogger->Scopes[m_NumberOfScopes].Parameters.RectPlotPix.top    = pgDataLogger->Scopes[m_NumberOfScopes].Parameters.RectDCPix.top + TextMetric.tmExternalLeading + TextMetric.tmHeight + 2;
	pgDataLogger->Scopes[m_NumberOfScopes].Parameters.RectPlotPix.right  = pgDataLogger->Scopes[m_NumberOfScopes].Parameters.RectDCPix.right - (2*TextMetric.tmMaxCharWidth + 2);
	pgDataLogger->Scopes[m_NumberOfScopes].Parameters.RectPlotPix.bottom = pgDataLogger->Scopes[m_NumberOfScopes].Parameters.RectDCPix.bottom - TextMetric.tmExternalLeading - 2*TextMetric.tmHeight - 2;

	// Fonts, Pens, Brushs.
	pgDataLogger->Scopes[m_NumberOfScopes].Parameters.TextFont.CreateFont( 
			-10, 
			0, 
			0, 
			0, 
			FW_SEMIBOLD, 
			FALSE, 
			FALSE, 
			0, 
			ANSI_CHARSET, 
			OUT_DEFAULT_PRECIS, 
			CLIP_DEFAULT_PRECIS, 
			DEFAULT_QUALITY, 
			DEFAULT_PITCH | FF_SWISS, 
			"Arial");
	
	pgDataLogger->Scopes[m_NumberOfScopes].Parameters.TextFontRotated.CreateFont( 
			-10, 
			0, 
			900, 
			0, 
			FW_SEMIBOLD, 
			FALSE, 
			FALSE, 
			0, 
			ANSI_CHARSET, 
			OUT_DEFAULT_PRECIS, 
			CLIP_DEFAULT_PRECIS, 
			DEFAULT_QUALITY, 
			DEFAULT_PITCH | FF_SWISS, 
			"Arial");

	pgDataLogger->Scopes[m_NumberOfScopes].Parameters.ScopePen.CreatePen( 
		PS_SOLID, 
		1, 
		pgDataLogger->Scopes[m_NumberOfScopes].Parameters.ScopeColor);
	
	pgDataLogger->Scopes[m_NumberOfScopes].Parameters.ScopeBrush.CreateSolidBrush(
		pgDataLogger->Scopes[m_NumberOfScopes].Parameters.ScopeColor);

	pgDataLogger->Scopes[m_NumberOfScopes].Parameters.BackgroundBrush.CreateSolidBrush(
		pgDataLogger->Scopes[m_NumberOfScopes].Parameters.BackgroundColor);

	// Increment the number of scopes:
	++pgDataLogger->m_NumberOfScopes;

	return(TRUE);
}

void GDataLogger::ScopeClose(void)
{
	int i;

	for(int nscope=0;nscope <pgDataLogger->m_NumberOfScopes;++nscope){
		// Release DC:
		pgDataLogger->Scopes[nscope].pWnd->ReleaseDC(pgDataLogger->Scopes[nscope].pDC);
		// Clear inserted variables
		pgDataLogger->Scopes[nscope].NumberOfInsertedVariables = 0;
		// Default time source index
		pgDataLogger->Scopes[nscope].TimeSourceIndex = -1;
		// 
		for(i=0;i<GDATALOGGER_MAXVARIABLES;++i){
			pgDataLogger->Scopes[nscope].FirstPointPloted[i] = FALSE;
		}
	}
}

void GDataLogger::ScopeSetDefaultParameters(PGDATALOGGERSCOPEPARAMETERS pDataLoggerScopeParameters)
{
	pDataLoggerScopeParameters->X_DivNumber = 9;
	pDataLoggerScopeParameters->Y_DivNumber = 7;
	pDataLoggerScopeParameters->X_Offset   = 0.0;
	pDataLoggerScopeParameters->X_DivScale = 1.0;
	pDataLoggerScopeParameters->Y_Offset   = 0.0;
	pDataLoggerScopeParameters->Y_DivScale = 1.0;
	pDataLoggerScopeParameters->BackgroundColor = RGB(255,255,255);
	pDataLoggerScopeParameters->ScopeColor = RGB(0,0,255);
	pDataLoggerScopeParameters->TextColor = RGB(128,0,64);
}

int GDataLogger::ScopeDraw(int nIDC, int FlagErase)
{
	int nscope;
	
	// Get scope index from IDC
	if((nscope = pgDataLogger->ScopeGetIndex(nIDC))<0){
		return(FALSE);
	}
	if(FlagErase)
		pgDataLogger->ScopeDrawAxis(nscope);
	pgDataLogger->ScopeDrawPlotVariables(nscope);
	return(TRUE);
}

int GDataLogger::ScopeInsertVariable(int nIDC, char *varname)
{
	int scopeindex,varindex;
	
	// Get scope index from IDC
	if((scopeindex = pgDataLogger->ScopeGetIndex(nIDC))<0){
		return(FALSE);
	}
	
	if (pgDataLogger->Scopes[scopeindex].NumberOfInsertedVariables>=(GDATALOGGER_MAXVARIABLES-1)){
		return(FALSE);
	}

	// Get variable index from varname
	if((varindex = pgDataLogger->VariableGetIndex(varname))<0){
		return(FALSE);
	}

	pgDataLogger->Scopes[scopeindex].InsertedVariablesIndex[pgDataLogger->Scopes[scopeindex].NumberOfInsertedVariables] = varindex;
	pgDataLogger->Scopes[scopeindex].FirstPointPloted[pgDataLogger->Scopes[scopeindex].NumberOfInsertedVariables] = FALSE;

	++pgDataLogger->Scopes[scopeindex].NumberOfInsertedVariables;

	return(TRUE);
}

int GDataLogger::ScopeDefineTimeSource(int nIDC, char *varname)
{
	int scopeindex,varindex;
	
	// Get scope index from IDC
	if((scopeindex = pgDataLogger->ScopeGetIndex(nIDC))<0){
		return(FALSE);
	}
	
	// Get variable index from varname
	if (varname != NULL){
		if((varindex = pgDataLogger->VariableGetIndex(varname))<0){
			return(FALSE);
		}
		pgDataLogger->Scopes[scopeindex].TimeSourceIndex = varindex;
	}
	else{
		pgDataLogger->Scopes[scopeindex].TimeSourceIndex = -1;
	}

	return(TRUE);
}

void TextOutRotated(CDC* pDC, const CString str, int x, int y, double angle)
{
   // convert angle to radian
   double pi = 3.141592654;
   double radian = pi * 2 / 360 * angle;
 
   // get the center of a not-rotated text
   CSize TextSize = pDC->GetTextExtent(str);
   CPoint center;
   center.x = TextSize.cx / 2;
   center.y = TextSize.cy / 2;
 
   // now calculate the center of the rotated text
   CPoint rcenter;
   rcenter.x = long(cos(radian) * center.x - sin(radian) * center.y);
   rcenter.y = long(sin(radian) * center.x + cos(radian) * center.y);
 
   // finally draw the text and move it to the center of the rectangle
   pDC->SetTextAlign(TA_BASELINE);
   pDC->SetBkMode(TRANSPARENT);
   pDC->TextOut(x - rcenter.x, y + rcenter.y, str);
}

int GDataLogger::ScopeDrawAxis(int nscope)
{
	CPen *pScopePen;
	CPen *pOldPen;
	CBrush *pScopeBrush;
	CBrush *pBackgroundBrush;
	CBrush *pOldBrush;
	CFont *pTextFont;
	CFont *pTextFontRotated;
	CFont *pOldFont;
	COLORREF *pTextColor;
	CRect *pRectPlotPix;
	CRect *pRectDCPix;
	double X_DivScale; 
	double Y_DivScale; 
	double X_Offset; 
	double Y_Offset; 
	int X_DivNumber;
	int	Y_DivNumber;

	double Xmin, Xmax, Ymin, Ymax;

	CString Strg;
	POINT Points[10];
	CRgn XArrow, YArrow;
	int i,j;

	CWnd *pWnd;
	CDC  *pDC;

	pWnd = pgDataLogger->Scopes[nscope].pWnd;
	pDC  = pgDataLogger->Scopes[nscope].pDC;
	pScopePen = &pgDataLogger->Scopes[nscope].Parameters.ScopePen;
	pScopeBrush = &pgDataLogger->Scopes[nscope].Parameters.ScopeBrush;
	pBackgroundBrush = &pgDataLogger->Scopes[nscope].Parameters.BackgroundBrush;
	pTextFont = &pgDataLogger->Scopes[nscope].Parameters.TextFont;
	pTextFontRotated = &pgDataLogger->Scopes[nscope].Parameters.TextFontRotated;
	pTextColor = &pgDataLogger->Scopes[nscope].Parameters.TextColor;
	pRectPlotPix = &pgDataLogger->Scopes[nscope].Parameters.RectPlotPix;
	pRectDCPix = &pgDataLogger->Scopes[nscope].Parameters.RectDCPix;
	X_DivScale = pgDataLogger->Scopes[nscope].Parameters.X_DivScale;
	Y_DivScale = pgDataLogger->Scopes[nscope].Parameters.Y_DivScale;
	X_Offset = pgDataLogger->Scopes[nscope].Parameters.X_Offset;
	Y_Offset = pgDataLogger->Scopes[nscope].Parameters.Y_Offset;
	X_DivNumber = pgDataLogger->Scopes[nscope].Parameters.X_DivNumber; 
	Y_DivNumber = pgDataLogger->Scopes[nscope].Parameters.Y_DivNumber; 

	Xmin = X_Offset - (X_DivNumber/2)* X_DivScale;
	Xmax = X_Offset + (X_DivNumber/2)* X_DivScale;
	Ymin = Y_Offset - (Y_DivNumber/2)* Y_DivScale;
	Ymax = Y_Offset + (Y_DivNumber/2)* Y_DivScale;

	// Save current drawing objects
	pOldPen = (CPen*) pDC->GetCurrentPen(); 
	pOldFont = (CFont*) pDC->GetCurrentFont(); 
	pOldBrush = (CBrush*) pDC->GetCurrentBrush(); 

	// Draw Background:
	pDC->SelectObject(pBackgroundBrush);
	pDC->Rectangle(*pRectDCPix);

	// Draw Axis: 
	pDC->SetTextColor(*pTextColor);
	pDC->SelectObject(pTextFont);
	pDC->SelectObject(pScopePen);
	pDC->SelectObject(pScopeBrush);
	pDC->MoveTo(pRectPlotPix->left,pRectPlotPix->bottom);
	pDC->LineTo(pRectPlotPix->right,pRectPlotPix->bottom);
	pDC->MoveTo(pRectPlotPix->left,pRectPlotPix->bottom);
	pDC->LineTo(pRectPlotPix->left,pRectPlotPix->top);
	
	Points[0].x = pRectPlotPix->right + 10;	Points[0].y = pRectPlotPix->bottom - 5;
	Points[1].x = pRectPlotPix->right + 15;	Points[1].y = pRectPlotPix->bottom;
	Points[2].x = pRectPlotPix->right + 10;	Points[2].y = pRectPlotPix->bottom + 5;
	XArrow.CreatePolygonRgn( Points, 3, WINDING);
	Points[0].x = pRectPlotPix->left - 5;	Points[0].y = pRectPlotPix->top - 10;
	Points[1].x = pRectPlotPix->left;		Points[1].y = pRectPlotPix->top - 15;
	Points[2].x = pRectPlotPix->left + 5;	Points[2].y = pRectPlotPix->top - 10;
	YArrow.CreatePolygonRgn( Points, 3, WINDING);
	pDC->MoveTo(pRectPlotPix->right,pRectPlotPix->bottom);
	pDC->LineTo(pRectPlotPix->right + 10,pRectPlotPix->bottom);
	pDC->MoveTo(pRectPlotPix->left,pRectPlotPix->top);
	pDC->LineTo(pRectPlotPix->left,pRectPlotPix->top - 11);
	pDC->SelectStockObject(BLACK_BRUSH);
	pDC->PaintRgn(&XArrow);
	pDC->PaintRgn(&YArrow);

	// Grades:
	pDC->SetTextAlign(TA_CENTER | TA_TOP);
	for (i=0; i<X_DivNumber; ++i){
		pDC->MoveTo(
			pRectPlotPix->left + (i*(pRectPlotPix->right-pRectPlotPix->left))/(X_DivNumber-1),
			pRectPlotPix->bottom-3);
		pDC->LineTo(
			pRectPlotPix->left + (i*(pRectPlotPix->right-pRectPlotPix->left))/(X_DivNumber-1),
			pRectPlotPix->bottom + 3);
		Strg.Format("%1.1f",Xmin + i*(Xmax-Xmin)/(X_DivNumber-1));
		pDC->TextOut(
			pRectPlotPix->left + (i*(pRectPlotPix->right-pRectPlotPix->left))/(X_DivNumber-1),
			pRectPlotPix->bottom + 3,
			Strg);
	}
	pDC->SetTextAlign(TA_RIGHT | TA_BASELINE);
	for (i=0; i<Y_DivNumber; ++i){
		pDC->MoveTo(pRectPlotPix->left - 3,
					pRectPlotPix->bottom - (i*(pRectPlotPix->bottom-pRectPlotPix->top))/(Y_DivNumber-1));
		pDC->LineTo(pRectPlotPix->left + 3,
					pRectPlotPix->bottom - (i*(pRectPlotPix->bottom-pRectPlotPix->top))/(Y_DivNumber-1));
		Strg.Format("%1.1f",Ymin + i*(Ymax-Ymin)/(Y_DivNumber-1));
		pDC->TextOut(pRectPlotPix->left - 5,
					 pRectPlotPix->bottom - (i*(pRectPlotPix->bottom-pRectPlotPix->top))/(Y_DivNumber-1),
					 Strg);
	}

	// Labels:
	if (pgDataLogger->Scopes[nscope].TimeSourceIndex >= 0){
		Strg.Format("%s[%s]",pgDataLogger->Variables[pgDataLogger->Scopes[nscope].TimeSourceIndex].VariableName,pgDataLogger->Variables[pgDataLogger->Scopes[nscope].TimeSourceIndex].VariableUnit);
	}
	else{
		Strg.Format("index");
	}
	pDC->SetTextAlign(TA_CENTER | TA_BASELINE);
	pDC->TextOut(pRectDCPix->left + (pRectDCPix->right-pRectDCPix->left)/2,
			pRectDCPix->bottom - 5,
			Strg);

	pDC->SelectObject(pTextFontRotated);
	Strg.Format("");
	for(i=0; i<pgDataLogger->Scopes[nscope].NumberOfInsertedVariables; ++i){
		j = pgDataLogger->Scopes[nscope].InsertedVariablesIndex[i];
		Strg.Format("%s %s[%s]",LPCTSTR(Strg),pgDataLogger->Variables[j].VariableName,pgDataLogger->Variables[j].VariableUnit);
	}
	pDC->SetTextAlign(TA_CENTER | TA_BASELINE);
	TextOutRotated(pDC, Strg, pRectDCPix->left + 5,
				 pRectDCPix->bottom + (pRectDCPix->top - pRectDCPix->bottom)/2,
				 90.0);
	// Restore previous drawing objects
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldFont);

	return 1;
}


void GDataLogger::ScopeDrawPlotVariables(int nscope)
{
	CPen PenData;
	CPen *pPreviousPen;
	int i,nvar,ninsertedvar,queueindex;
	int Xpix, Ypix;
	double Xpixfloat, Ypixfloat;
	double X_DivScale; 
	double Y_DivScale; 
	double X_Offset; 
	double Y_Offset; 
	int X_DivNumber;
	int	Y_DivNumber;
	double Xmin, Xmax, Ymin, Ymax, X, Y;
	CRect *pRectPlotPix;
	CWnd *pWnd;
	CDC  *pDC;

	pWnd = pgDataLogger->Scopes[nscope].pWnd;
	pDC  = pgDataLogger->Scopes[nscope].pDC;

	X_DivScale = pgDataLogger->Scopes[nscope].Parameters.X_DivScale;
	Y_DivScale = pgDataLogger->Scopes[nscope].Parameters.Y_DivScale;
	X_Offset = pgDataLogger->Scopes[nscope].Parameters.X_Offset;
	Y_Offset = pgDataLogger->Scopes[nscope].Parameters.Y_Offset;
	X_DivNumber = pgDataLogger->Scopes[nscope].Parameters.X_DivNumber; 
	Y_DivNumber = pgDataLogger->Scopes[nscope].Parameters.Y_DivNumber; 
	pRectPlotPix = &pgDataLogger->Scopes[nscope].Parameters.RectPlotPix;

	Xmin = X_Offset - (X_DivNumber/2)* X_DivScale;
	Xmax = X_Offset + (X_DivNumber/2)* X_DivScale;
	Ymin = Y_Offset - (Y_DivNumber/2)* Y_DivScale;
	Ymax = Y_Offset + (Y_DivNumber/2)* Y_DivScale;

	pPreviousPen = (CPen*) pDC->GetCurrentPen(); 

	for(ninsertedvar=0;ninsertedvar<pgDataLogger->Scopes[nscope].NumberOfInsertedVariables;++ninsertedvar){
		// Get varindex:
		nvar = pgDataLogger->Scopes[nscope].InsertedVariablesIndex[ninsertedvar];
		// Pen
		PenData.CreatePen( PS_SOLID, 1, RGB(255,0,0));
		pDC->SelectObject(&PenData);
		// Empty queue:
		i = 0;
		while(gQUEUE_RequestReadIndex(&pgDataLogger->Variables[nvar].CircularQueueControl, QUEUE_DESTINATION_SCOPE, &queueindex)){
			Y = pgDataLogger->Variables[nvar].CircularQueue[queueindex];
			if (pgDataLogger->Scopes[nscope].TimeSourceIndex >= 0){
				X = pgDataLogger->Variables[pgDataLogger->Scopes[nscope].TimeSourceIndex].CircularQueue[queueindex];
			}
			else{
				break;
			}
			if ( (X>=Xmin) && (X<=Xmax) && (Y>=Ymin) && (Y<=Ymax) ){
				Xpixfloat = (X-Xmin)/(Xmax-Xmin)*(pRectPlotPix->right-pRectPlotPix->left);
				Ypixfloat = (Y-Ymin)/(Ymax-Ymin)*(pRectPlotPix->bottom-pRectPlotPix->top);
				Xpix = (int)(Xpixfloat);
				Ypix = (int)(Ypixfloat);
				if (pgDataLogger->Scopes[nscope].FirstPointPloted[ninsertedvar]){
					pDC->MoveTo(pRectPlotPix->left + pgDataLogger->Scopes[nscope].PreviousXpix[ninsertedvar], pRectPlotPix->bottom - pgDataLogger->Scopes[nscope].PreviousYpix[ninsertedvar]);
					pDC->LineTo(pRectPlotPix->left + Xpix, pRectPlotPix->bottom - Ypix);
				}
				pgDataLogger->Scopes[nscope].PreviousXpix[ninsertedvar] = Xpix;
				pgDataLogger->Scopes[nscope].PreviousYpix[ninsertedvar] = Ypix;
				pgDataLogger->Scopes[nscope].FirstPointPloted[ninsertedvar] = TRUE;
				++i;
			}
		}
	}

	pDC->SelectObject(pPreviousPen);
}

int GDataLogger::ScopeGetIndex(int nIDC)
{
	for(int n=0;n<pgDataLogger->m_NumberOfScopes;++n){
		if(pgDataLogger->Scopes[n].nIDC==nIDC){
			return(n);
		}
	}
	return(-1);
}
*/
