/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "It_posix_pthread.h"

static VOID *pthread_f01(VOID *arg)
{
    INT32 ret = 0;
    INT32 policy;
    INT32 priority;
    struct sched_param schedparam;

    LosTaskDelay(2);

#ifdef LOSCFG_KERNEL_TICKLESS
    priority = sched_get_priority_max(SCHED_RR) - 1;
#else
    priority = sched_get_priority_max(SCHED_RR);                  // priority = 31
#endif

    ret = pthread_getschedparam(pthread_self(), &policy, &schedparam);
    ICUNIT_TRACK_EQUAL(ret, PTHREAD_NO_ERROR, ret);
    ICUNIT_TRACK_EQUAL(policy, SCHED_RR, policy);
    ICUNIT_TRACK_EQUAL(schedparam.sched_priority, priority, schedparam.sched_priority);

    return NULL;
}

static VOID *pthread_f02(VOID *arg)
{
    INT32 ret = 0;
    struct sched_param schedparam;
#ifdef LOSCFG_KERNEL_TICKLESS
    schedparam.sched_priority = sched_get_priority_max(SCHED_RR) - 1;
    ICUNIT_TRACK_EQUAL(schedparam.sched_priority, OS_TASK_PRIORITY_LOWEST - 1, schedparam.sched_priority);
#else
    schedparam.sched_priority = sched_get_priority_max(SCHED_RR); // priority = 31
    ICUNIT_TRACK_EQUAL(schedparam.sched_priority, OS_TASK_PRIORITY_LOWEST, schedparam.sched_priority);
#endif

    ret = pthread_setschedparam(*(pthread_t *)arg, SCHED_RR, &schedparam);
    ICUNIT_TRACK_EQUAL(ret, PTHREAD_NO_ERROR, ret);

    return NULL;
}

static UINT32 Testcase(VOID)
{
    INT32 ret = 0;
    pthread_t newTh1, newTh2;
    pthread_attr_t attr;
    INT32 bar;

    ret = pthread_attr_init(&attr);
    ICUNIT_ASSERT_EQUAL(ret, PTHREAD_NO_ERROR, ret);

    g_pthreadSchedInherit = PTHREAD_EXPLICIT_SCHED;
    ret = pthread_attr_setinheritsched(&attr, g_pthreadSchedInherit);
    ICUNIT_ASSERT_EQUAL(ret, PTHREAD_NO_ERROR, ret);

    LOS_TaskLock();
    ret = pthread_create(&newTh1, &attr, pthread_f01, NULL);
    ICUNIT_ASSERT_EQUAL(ret, PTHREAD_NO_ERROR, ret);

    ret = pthread_create(&newTh2, &attr, pthread_f02, &newTh1);
    ICUNIT_ASSERT_EQUAL(ret, PTHREAD_NO_ERROR, ret);

    LOS_TaskUnlock();

    ret = pthread_join(newTh2, NULL);
    ICUNIT_ASSERT_EQUAL(ret, PTHREAD_NO_ERROR, ret);

    ret = pthread_join(newTh1, NULL);
    ICUNIT_ASSERT_EQUAL(ret, PTHREAD_NO_ERROR, ret);

    ret = pthread_attr_destroy(&attr);
    ICUNIT_ASSERT_EQUAL(ret, PTHREAD_NO_ERROR, ret);

    return PTHREAD_NO_ERROR;
}


VOID ItPosixPthread205(VOID)
{
    TEST_ADD_CASE("IT_POSIX_PTHREAD_205", Testcase, TEST_POSIX, TEST_PTHREAD, TEST_LEVEL2, TEST_FUNCTION);
}
