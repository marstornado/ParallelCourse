#include "queue.h"
#include <pthread.h>

pthread_mutex_t mutexl = PTHREAD_MUTEX_INITIALIZER;

#define LockQOpen() ({                \
    pthread_mutex_lock(&mutexl);      \
    void *locQBegin = qopen();        \
    pthread_mutex_unlock(&mutexl);    \
    locQBegin;                        \
})

#define LockQPut(qp, elementp) ({     \
    pthread_mutex_lock(&mutexl);      \
    qput(qp, elementp);               \
    pthread_mutex_unlock(&mutexl);    \
})

#define LockQGet(qp) ({               \
    pthread_mutex_lock(&mutexl);      \
    void *elementp = qget(qp);        \
    pthread_mutex_unlock(&mutexl);    \
    elementp;                         \
})

#define LockQApply(qp, fn) ({         \
    pthread_mutex_lock(&mutexl);      \
    qapply(qp, fn);                   \
    pthread_mutex_unlock(&mutexl);    \
})

#define LockQSearch(qp, searchfn, keyp) ({         \
    pthread_mutex_lock(&mutexl);                   \
    void *searchp = qsearch(qp, searchfn, keyp);   \
    pthread_mutex_unlock(&mutexl);                 \
    searchp;                                       \
}) 

#define LockQRemove(qp, searchfn, keyp) ({         \
    pthread_mutex_lock(&mutexl);                   \
    qremove(qp, searchfn, keyp);                   \
    pthread_mutex_unlock(&mutexl);                 \
})

