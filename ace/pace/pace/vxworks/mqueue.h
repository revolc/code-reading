/* mqueue.h,v 1.1 2001/06/01 20:56:48 joeh Exp

 * ============================================================================
 *
 * = LIBRARY
 *    pace
 *
 * = FILENAME
 *    pace/vxworks/mqueue.h
 *
 * = AUTHOR
 *    Joe Hoffert
 *
 * ============================================================================ */

#ifndef PACE_MQUEUE_H_VXWORKS
#define PACE_MQUEUE_H_VXWORKS

#if PACE_LINUX
# include "pace/emulation/mqueue.h"
#else
# include <mqueue.h>
#endif /* PACE_LINUX */

#if defined (PACE_HAS_CPLUSPLUS)
extern "C" {
#endif /* PACE_HAS_CPLUSPLUS */

#ifndef PACE_MQD_T
#define PACE_MQD_T
  typedef mqd_t pace_mqd_t;
#endif /* PACE_MQD_T */

#ifndef PACE_MQ_ATTR
#define PACE_MQ_ATTR
  typedef struct mq_attr pace_mq_attr;
#endif /* PACE_MQ_ATTR */

#if defined (PACE_HAS_CPLUSPLUS)
}
#endif /* PACE_HAS_CPLUSPLUS */

#endif /* PACE_MQUEUE_H_VXWORKS */
