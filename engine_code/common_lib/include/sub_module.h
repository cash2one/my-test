#ifndef _SUB_MODULE_H
#define _SUB_MODULE_H

/*when you use trace module,the ONLY codes to be modifited is here:
	enmu type var: 		the module ID ,also used for name index 
	module_type_name:	the sub module name used in your project
*/




/* ISwalker: USING Attention!!!!!!!!
	 the ENUM and name arry MUST keep the SAME order !!!!
	 Queue Please ,:) Aha
*/
//enum _module_type_t{
//	MODULE_MIN=0,	/* keep untouched */

	/* the following is your sub module definition*/
/* 	MAIN_ENTRY = MODULE_MIN,
 * 	SECOND_EVT,
 * 	MERGE_EVT,
 * 	FW_IACT	,
 * 	IDS_BLK	,
 * 	EVT_FILTER,
 * 	GET_POLICY,
 */
/*         FILE_OPERATION = 0,
 * 	AUTH,
 * 	NETWORK_COMMU,
 * 	FIREWALL_LINKAGE,
 * 	SHM_COMMU,
 * 	ACCREDIT,
 * 	RUN_LOG,                        // ÔËÐÐÈÕÖ¾
 * 	AUDIT_LOG,                      // Éó¼ÆÈÕÖ¾
 * 	STAT_LOG,                        // Í³¼ÆÈÕÖ¾
 */
	/*other modules can be added here*/

//	MODULE_MAX	/* keep untouched !! */
//}module_type_t;

/* #define MODULE_ALL	( 1<<MAIN_ENTRY|1<<SECOND_EVT|1<<MERGE_EVT \
 * 			  |1<<FW_IACT|1<<IDS_BLK|1<<EVT_FILTER \
 * 			  |1<<GET_POLICY )
 */


/* #define MODULE_ALL	( 1<<FILE_OPERATION|1<<AUTH|1<<NETWORK_COMMU \
 * 			  |1<<FIREWALL_LINKAGE|1<<SHM_COMMU|1<<ACCREDIT \
 * 			  |1<<RUN_LOG|1<<AUDIT_LOG|1<<STAT_LOG )
 * 
 * 
 * static char * module_type_name[]={
 * 	 "file_operation",
 * 	 "auth",
 * 	 "network_commu",
 * 	 "firewall_linkage",
 * 	 "shm_commu",
 * 	 "accredit",
 * 	 "run_log",
 * 	 "audit_log",
 * 	 "stat_log",
 */
	/*other modules name can be adde here*/

	/**/
//};


/* #define TABLE_SIZE(table) (sizeof(table)/sizeof(table[0])) 
 * 
 * 
 * 
 * static inline char *module_mask_2name(unsigned int mask)
 * {
 * 	static char *p = "dummy";
 * 
 * 	if(mask < MODULE_MIN || mask >MODULE_MAX) {
 * 		fprintf(stderr,"invalid module mask!\n");	
 * 		return p;
 * 	} 
 * 	
 * 	return module_type_name[mask];
 * }
 * 
 * static inline int module_name2id(char *module)
 * {
 * 	int module_mask = 0;	
 * 	int i = 0;
 * 
 * 	if ( !module || '\0' == module[0] ) {
 * 		fprintf(stderr,"module name(%s) invalid!\n",module);
 * 		goto ret;
 * 	}
 * 
 * 	for ( i = 0; i < TABLE_SIZE(module_type_name); i++){
 * #ifdef VENUS_TRACE_DEBUG
 * 		printf("the module_type_name[%d]:%s\n",i,module_type_name[i]);
 * #endif
 * 		if (!strcasecmp(module_type_name[i],module)) {
 * 			return (1<<i);
 * 		}
 * 	}
 * 
 * ret:
 * 	return module_mask;
 * }
 * 
 * 
 */
//#endif	/*end*/
