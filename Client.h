/*  File Name: Client.h
 *  Unit Code: CAB403
 *  @author Samuel Janetzki (n7402643)
 *  @version v0.0
 *  @brief Description
 */

#include "Constants.h"

/** @brief Description
 *  @param variable_name Description of variable
 *  @return Description of return
 */
int welcome_login(int sock);

/** @brief Description
 *  @param variable_name Description of variable
 *  @return Description of return
 */
int talking(int sock, char *sen, char *rec);

/** @brief Description
 *  @param variable_name Description of variable
 *  @return Description of return
 */
int auto_mach_tell(int sock);
