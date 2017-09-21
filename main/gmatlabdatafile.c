/*****************************************************************************
// File: gMATLABDataFile.c 
// Contents: Functions for writing in .mat files (matlab) version 4.0.
// Author: G. A. Borges.
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "gmatlabdatafile.h"

typedef struct{
	uint32_t type;
	uint32_t mrows;
	uint32_t ncols;
	uint32_t imagf;
	uint32_t namlen;
} MATLAB_DATAHEAD, *PMATLAB_DATAHEAD;

int gMATLABDataFile_OpenWrite(PGMATLABDATAFILECONFIG pGMatlabDataFileConfig, char *filename, char *dirname)
{
	//sprintf(pGMatlabDataFileConfig->FileName,"");
	pGMatlabDataFileConfig->FileName[0] = '\0';
	if(dirname!=NULL)
		sprintf(pGMatlabDataFileConfig->FileName,"%s",dirname);
	strcat(pGMatlabDataFileConfig->FileName,filename);
	pGMatlabDataFileConfig->FlagStillNotSaved = TRUE;

	pGMatlabDataFileConfig->fp = fopen(pGMatlabDataFileConfig->FileName, "w");
/*	pGMatlabDataFileConfig->fp = CreateFile(pGMatlabDataFileConfig->FileName,
                        GENERIC_WRITE,
                        0,
                        NULL,
                        CREATE_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL); */
	if(pGMatlabDataFileConfig->fp==NULL){
		printf("\n pGMatlabDataFileConfig->fp==NULL: %s",pGMatlabDataFileConfig->FileName);
//		MessageBoxLastError();
		return FALSE;
	}
	rewind(pGMatlabDataFileConfig->fp);

	return TRUE;

}

int gMATLABDataFile_OpenRead(PGMATLABDATAFILECONFIG pGMatlabDataFileConfig, char *filename, char *dirname)
{
	//sprintf(pGMatlabDataFileConfig->FileName,"");
	pGMatlabDataFileConfig->FileName[0] = '\0';
	if(dirname!=NULL)
		sprintf(pGMatlabDataFileConfig->FileName,"%s",dirname);
		
	strcat(pGMatlabDataFileConfig->FileName,filename);
	pGMatlabDataFileConfig->FlagStillNotSaved = TRUE;

	pGMatlabDataFileConfig->fp = fopen(pGMatlabDataFileConfig->FileName, "r");
/*	pGMatlabDataFileConfig->fp = CreateFile(pGMatlabDataFileConfig->FileName,
                        GENERIC_READ,
                        0,
                        NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);
*/	if(pGMatlabDataFileConfig->fp==NULL){
//		MessageBoxLastError();
		return FALSE;
	}
	rewind(pGMatlabDataFileConfig->fp);

	return TRUE;

}

void gMATLABDataFile_Close(PGMATLABDATAFILECONFIG pGMatlabDataFileConfig)
{
	fclose(pGMatlabDataFileConfig->fp);

}

int gMATLABDataFile_SaveVector(PGMATLABDATAFILECONFIG pGMatlabDataFileConfig, const char *varname, double *v, uint32_t nlin)
{
	MATLAB_DATAHEAD DataHead;

	DataHead.type  = (uint32_t)(0);		// Double.
	DataHead.mrows = (uint32_t)(nlin);	
	DataHead.ncols = (uint32_t)(1);
	DataHead.imagf = (uint32_t)(0);
	DataHead.namlen = (uint32_t)(strlen(varname)+1);

	if(! fwrite(&DataHead, sizeof(MATLAB_DATAHEAD), 1, pGMatlabDataFileConfig->fp) ){
//		MessageBoxLastError();
		return FALSE;
	}

	if(! fwrite(varname, sizeof(char), (strlen(varname)+1), pGMatlabDataFileConfig->fp)){
//		MessageBoxLastError();
		return FALSE;
	}

	if(! fwrite(v, sizeof(double), nlin, pGMatlabDataFileConfig->fp)){
//		MessageBoxLastError();
		return FALSE;
	}

	pGMatlabDataFileConfig->FlagStillNotSaved = FALSE;

	return(TRUE);

}

int gMATLABDataFile_SaveMatrix(PGMATLABDATAFILECONFIG pGMatlabDataFileConfig, const char *varname, double **m, uint32_t nlin, uint32_t ncol)
{
	MATLAB_DATAHEAD DataHead;
	uint32_t nl,nc;
	double *vcol;

	DataHead.type  = (uint32_t)(0);		// Double.
	DataHead.mrows = (uint32_t)(nlin);	
	DataHead.ncols = (uint32_t)(ncol);
	DataHead.imagf = (uint32_t)(0);
	DataHead.namlen = (uint32_t)(strlen(varname)+1);

	if(! fwrite(&DataHead, sizeof(MATLAB_DATAHEAD), 1, pGMatlabDataFileConfig->fp) ){
//		MessageBoxLastError();
		return FALSE;
	}

	if(! fwrite(varname, sizeof(char), (strlen(varname)+1), pGMatlabDataFileConfig->fp)){
//		MessageBoxLastError();
		return FALSE;
	}

	vcol = (double*)malloc(nlin*sizeof(double));

	for(nc=0;nc<ncol;++nc){
		for(nl=0;nl<nlin;++nl){
			vcol[nl] = m[nl][nc];
		}
		if(! fwrite(vcol, sizeof(double), nlin, pGMatlabDataFileConfig->fp)){
		//MessageBoxLastError();
			free(vcol);
			return FALSE;
		}
	}
	free(vcol);

	pGMatlabDataFileConfig->FlagStillNotSaved = FALSE;

	return(TRUE);

}
