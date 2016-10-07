/*  File Name: Server.h
 *  Unit Code: CAB403
 *  @author Samuel Janetzki (n7402643)
 *  @version v0.0
 *  @brief Description
 */

#include "Constants.h"

/** @brief Loop to read and handle client actions
 *  @param id Client ID
 *  @return Continuation status
 */
int client_actions(int id);

/** @brief Loop to read and handle client actions
 *  @param id Client ID
 *  @return Continuation status
 */
void handler(char *sen, char *rec);

/** @brief Loop to read and handle client actions
 *  @param id Client ID
 *  @return Continuation status
 */
void extract_variables(char *var, char *str, char *rem);

/** @brief Interrupt handler for CTRL C
 *  @param dummy Dummy variable.
 *  @return void
 */
void interrupt(int dummy);