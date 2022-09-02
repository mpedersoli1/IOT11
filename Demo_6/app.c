#include <stdio.h>
#include <pthread.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

/* Local includes. */
#include "console.h"

#define TASK1_PRIORITY 0
#define TASK2_PRIORITY 1
#define TASK3_PRIORITY 2
#define TASK4_PRIORITY 0


#define BLACK "\033[30m" /* Black */
#define RED "\033[31m"   /* Red */
#define GREEN "\033[32m" /* Green */
#define DISABLE_CURSOR() printf("\e[?25l")
#define ENABLE_CURSOR() printf("\e[?25h")

#define clear() printf("\033[H\033[J")
#define gotoxy(x, y) printf("\033[%d;%dH", (y), (x))

typedef struct
{
    int pos;
    char *color;
    int period_ms;
} st_led_param_t;

st_led_param_t green = {
    6,
    GREEN,
    250};
st_led_param_t red = {
    13,
    RED,
    100};

TaskHandle_t greenTask_hdlr, redTask_hdlr;
QueueHandle_t structQueue = NULL;
int freeze = 0;

#include <termios.h>


static void prvTask_getChar(void *pvParameters)
{
    char key;
    int n;

    /* I need to change  the keyboard behavior to
    enable nonblock getchar */
    struct termios initial_settings,
        new_settings;

    tcgetattr(0, &initial_settings);

    new_settings = initial_settings;
    new_settings.c_lflag &= ~ICANON;
    new_settings.c_lflag &= ~ECHO;
    new_settings.c_lflag &= ~ISIG;
    new_settings.c_cc[VMIN] = 0;
    new_settings.c_cc[VTIME] = 1;

    tcsetattr(0, TCSANOW, &new_settings);
    /* End of keyboard configuration */
    for (;;)
    {
        int stop = 0;
        key = getchar();
        if( key=='1' ||key=='2' ||key=='3' ||key=='4' ||key=='5' ||key=='6' ||key=='7' ||key=='8' ||key=='9' ||key=='0')
        {
            vTaskResume(redTask_hdlr);
            key =0;
        }
        if (key > 0)
        {
            switch (key)
            {
            case 'k':
                stop = 1;
                break;
            case '*' :
                freeze=1;
                vTaskResume(redTask_hdlr);
                break;
            case '+':
                freeze = 0;
                vTaskResume(redTask_hdlr);
                vTaskResume(greenTask_hdlr);
                break;
            default:
                if (xQueueSend(structQueue, &key, 0) != pdTRUE)
                {
                    vTaskResume(redTask_hdlr);
                }
            }
        }
        if (stop)
        {
            break;
        }/*
        if(red_resume)
        {
            vTaskResume(redTask_hdlr);
        }*/
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    tcsetattr(0, TCSANOW, &initial_settings);
    ENABLE_CURSOR();
    exit(0);
    vTaskDelete(NULL);
}

static void prvTask_led(void *pvParameters)
{
    // pvParameters contains LED params
    st_led_param_t *led = (st_led_param_t *)pvParameters;
    portTickType xLastWakeTime = xTaskGetTickCount();

    vTaskSuspend(redTask_hdlr);
    
    
    for (;;)
    {
        if(pvParameters==&red && freeze==1)
        {   
            vTaskResume(redTask_hdlr);
        }
        
        // console_print("@");
        gotoxy(led->pos, 2);
        printf("%s⬤", led->color);
        fflush(stdout);
        vTaskDelay(led->period_ms  / portTICK_PERIOD_MS);
        // vTaskDelayUntil(&xLastWakeTime, led->period_ms / portTICK_PERIOD_MS);
        if(pvParameters==&red && freeze==1)
        {   
            vTaskSuspend(redTask_hdlr);
        }
        

        gotoxy(led->pos, 2);
        printf("%s ", BLACK);
        fflush(stdout);
        vTaskDelay(led->period_ms  / portTICK_PERIOD_MS);
        // vTaskDelayUntil(&xLastWakeTime, led->period_ms / portTICK_PERIOD_MS);
        if(pvParameters==&red)
        {   
            vTaskSuspend(redTask_hdlr);
        }
        if(pvParameters==&green && freeze==1)
        {   
            vTaskSuspend(greenTask_hdlr);
        }
        
    }

    vTaskDelete(NULL);
}



static void prvTask_logger(void *pvParameters)
{
    (void)pvParameters;
    char key;
    const int CURSOR_Y = 4;
    int cursor_x = 0;
    for (;;)
    {
        if (xQueueReceive(structQueue, &key, portMAX_DELAY) == pdPASS)
        {
            gotoxy(cursor_x++, CURSOR_Y);
            printf("%c", key);
            fflush(stdout);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void app_run(void)
{
    structQueue = xQueueCreate(10, // Queue length
                               1); // Queue item size

    if (structQueue == NULL)
    {
        printf("Fail on create queue\n");
        exit(1);
    }

    clear();
    DISABLE_CURSOR();
    printf(
        "╔═════════════════╗\n"
        "║                 ║\n"
        "╚═════════════════╝\n");
        //"aaaaaaaaaaaaa");

    xTaskCreate(prvTask_logger, "logger", configMINIMAL_STACK_SIZE, NULL, TASK4_PRIORITY, NULL);
    xTaskCreate(prvTask_led, "LED_green", configMINIMAL_STACK_SIZE, &green, TASK1_PRIORITY, &greenTask_hdlr);
    xTaskCreate(prvTask_led, "LED_red", configMINIMAL_STACK_SIZE, &red, TASK2_PRIORITY, &redTask_hdlr);
    xTaskCreate(prvTask_getChar, "Get_key", configMINIMAL_STACK_SIZE, NULL, TASK3_PRIORITY, NULL);

    /* Start the tasks and timer running. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following
     * line will never be reached.  If the following line does execute, then
     * there was insufficient FreeRTOS heap memory available for the idle and/or
     * timer tasks      to be created.  See the memory management section on the
     * FreeRTOS web site for more details. */
    for (;;)
    {
    }
}
