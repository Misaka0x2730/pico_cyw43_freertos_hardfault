#include "pico/multicore.h"
#include "pico/cyw43_arch.h"

#include "FreeRTOS.h"
#include "task.h"

#define ENABLE_CYW43_INIT_ISSUE

#if (CYW43_TASK_PRIORITY <= 1)
#error "For this test, CYW43_TASK_PRIORITY must be at least 2!"
#elif (CYW43_TASK_PRIORITY >= (configMAX_PRIORITIES - 1))
#error "For this test, CYW43_TASK_PRIORITY must be less than (configMAX_PRIORITIES - 1)"
#endif

#if defined(ENABLE_CYW43_INIT_ISSUE)
#define TEST_THREAD_PRIORITY  (CYW43_TASK_PRIORITY + 1)
#else
#define TEST_THREAD_PRIORITY  (CYW43_TASK_PRIORITY - 1)
#endif

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    (void)pcTaskName;
    (void)pxTask;

    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected. */

    assert(false);
}

_Noreturn static void test_thread(void* params)
{
    cyw43_arch_init();
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
    while (1);
}

void main(void)
{
    multicore_reset_core1();

    const BaseType_t result = xTaskCreate(test_thread,
                                          "test_thread",
                                          1024,
                                          NULL,
                                          TEST_THREAD_PRIORITY,
                                          NULL);

    assert(result == pdPASS);

    vTaskStartScheduler();

	assert(false);
}