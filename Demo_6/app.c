#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

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
volatile int freeze = 0;
volatile int red_enabled = 0;

#include <termios.h>

static struct termios g_initial_settings;
static int g_terminal_ready = 0;

static void cleanup_terminal(void)
{
    if (g_terminal_ready)
    {
        tcsetattr(0, TCSANOW, &g_initial_settings);
    }
    ENABLE_CURSOR();
}


static void prvTask_getChar(void *pvParameters)
{
    char key;

    /* I need to change  the keyboard behavior to
    enable nonblock getchar */
    struct termios new_settings;

    if (tcgetattr(0, &g_initial_settings) != 0)
    {
        perror("tcgetattr failed");
        cleanup_terminal();
        exit(1);
    }

    new_settings = g_initial_settings;
    new_settings.c_lflag &= ~ICANON;
    new_settings.c_lflag &= ~ECHO;
    new_settings.c_lflag &= ~ISIG;
    new_settings.c_cc[VMIN] = 0;
    new_settings.c_cc[VTIME] = 1;

    if (tcsetattr(0, TCSANOW, &new_settings) != 0)
    {
        perror("tcsetattr failed");
        cleanup_terminal();
        exit(1);
    }
    g_terminal_ready = 1;
    /* End of keyboard configuration */
    for (;;)
    {
        int stop = 0;
        key = getchar();
        if( key=='1' ||key=='2' ||key=='3' ||key=='4' ||key=='5' ||key=='6' ||key=='7' ||key=='8' ||key=='9' ||key=='0')
        {
            red_enabled = 1;
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
                red_enabled = 1;
                break;
            case '+':
                freeze = 0;
                break;
            default:
                if (xQueueSend(structQueue, &key, 0) != pdTRUE)
                {
                    /* Drop if queue is full. */
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
    cleanup_terminal();
    exit(0);
    vTaskDelete(NULL);
}

static void prvTask_led(void *pvParameters)
{
    // pvParameters contains LED params
    st_led_param_t *led = (st_led_param_t *)pvParameters;
    for (;;)
    {
        if ((pvParameters == &red && red_enabled == 0) ||
            (pvParameters == &green && freeze == 1))
        {
            vTaskDelay(25 / portTICK_PERIOD_MS);
            continue;
        }

        gotoxy(led->pos, 2);
        printf("%s⬤", led->color);
        fflush(stdout);
        vTaskDelay(led->period_ms  / portTICK_PERIOD_MS);

        gotoxy(led->pos, 2);
        printf("%s ", BLACK);
        fflush(stdout);
        vTaskDelay(led->period_ms  / portTICK_PERIOD_MS);

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
    atexit(cleanup_terminal);
    printf(
        "╔═════════════════╗\n"
        "║                 ║\n"
        "╚═════════════════╝\n");
        //"aaaaaaaaaaaaa");

    if (xTaskCreate(prvTask_logger, "logger", configMINIMAL_STACK_SIZE, NULL, TASK4_PRIORITY, NULL) != pdPASS ||
        xTaskCreate(prvTask_led, "LED_green", configMINIMAL_STACK_SIZE, &green, TASK1_PRIORITY, &greenTask_hdlr) != pdPASS ||
        xTaskCreate(prvTask_led, "LED_red", configMINIMAL_STACK_SIZE, &red, TASK2_PRIORITY, &redTask_hdlr) != pdPASS ||
        xTaskCreate(prvTask_getChar, "Get_key", configMINIMAL_STACK_SIZE, NULL, TASK3_PRIORITY, NULL) != pdPASS)
    {
        printf("Fail on create task\n");
        cleanup_terminal();
        exit(1);
    }

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
