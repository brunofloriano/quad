/*****************************************************************************
// File: GDataLogger.h
// Contents: Header for GDataLogger functions.
// Author: G. A. Borges.
*****************************************************************************/
#ifndef GDATALOGGER_H
#define GDATALOGGER_H

#ifdef __cplusplus
 extern "C" {
#endif 

#define GDATALOGGER_MAXVARIABLES 50

typedef struct{
	char VariableName[100];
	char VariableUnit[50];
	long int GMatlabDataFileIndex;
	int Nr;
	int Nc;
	double *CircularQueue;
	GQUEUECONTROL CircularQueueControl;
} GDATALOGGERVARIABLE,*PGDATALOGGERVARIABLE;

typedef struct{
	GDATALOGGERVARIABLE Variables[GDATALOGGER_MAXVARIABLES];
	int m_NumberOfVariables;
	GMATLABDATAFILECONFIG GMatlabDataFileConfig;
}GDATALOGGER,*PGDATALOGGER;

int gDataLogger_Init(PGDATALOGGER pgDataLogger, char *filename, char *dirname);
int gDataLogger_DeclareVariable(PGDATALOGGER pgDataLogger, char *varname, char *varunit, int nrows, int ncols, int queuesize);
int gDataLogger_InsertVariable(PGDATALOGGER pgDataLogger, char *varname, double *varvalue);
int gDataLogger_MatfileUpdate(PGDATALOGGER pgDataLogger);
int gDataLogger_Close(PGDATALOGGER pgDataLogger);

#ifdef __cplusplus
}
#endif 

#endif // GDATALOGGER_H
