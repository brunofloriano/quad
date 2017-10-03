/*****************************************************************************
*** Projeto CARCARAH (UnB-Expansion)
*** Conteudo: modulo time.
*** Autor: G. A. Borges.
*** Atualizacoes: 
	- 01/05/2009: criacao
*****************************************************************************/
/*! \file time.h 
* \brief Arquivo cabealho do modulo time. */
#ifndef GTIME_H
#define GTIME_H

#ifdef __cplusplus
 extern "C" {
#endif 

// Definicoes de uso externo:
typedef struct{
        struct timeval     timereset;
        struct timeval     time;
}timestruct_t;

// Prototipos de uso externo:

// Prototipos de uso externo (inline):
/*! \fn  inline void time_reset(timestruct_t *ptimestruct)
* Funcao que reinicia a contagem de tempo com a estrutura ptimestruct.
* \param 
* \return
*/
static __inline void time_reset(timestruct_t *ptimestruct)
{
	gettimeofday(&ptimestruct->timereset, NULL);
}                      

/*! \fn  inline double time_gettime(timestruct_t *ptimestruct)
* Funcao que retorna o tempo (em segundos) decorrido desde o ultimo reset da estrutura ptimestruct.
* \param 
* \return
*/
static __inline double time_gettime(timestruct_t *ptimestruct)
{
	gettimeofday(&ptimestruct->time, NULL);
	return (ptimestruct->time.tv_sec - ptimestruct->timereset.tv_sec) + 1e-6 * (ptimestruct->time.tv_usec - ptimestruct->timereset.tv_usec);
}                      


#ifdef __cplusplus
}
#endif 

#endif // GTIME_H
