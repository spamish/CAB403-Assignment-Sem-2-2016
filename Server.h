/*  File Name: Server.h
 *  Unit Code: CAB403
 *  @author Samuel Janetzki (n7402643)
 *  @version v0.0
 *  @brief Description
 */

#include "Constants.h"

/*  @brief 
 *  @param 
 *  @return 
 */
int get_lines(FILE *data);

/*  @brief Loop to read and handle client actions
 *  @param id Client ID
 *  @return Continuation status
 */
int client_actions(int id);

/*  @brief 
 *  @param 
 *  @return 
 */
void handler(char *sen, char *rec);

/*  @brief 
 *  @param 
 *  @return 
 */
void extract_variables(char *var, char *str, char *rem);

/*  @brief 
 *  @param 
 *  @return 
 */
int authenticate_client(char *username, char *password);

/*  @brief Interrupt handler for CTRL C
 *  @param dummy Dummy variable.
 *  @return void
 */
void interrupt(int dummy);