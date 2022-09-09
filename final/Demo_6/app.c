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

#define PRIORITY_0 0
#define PRIORITY_1 1
#define PRIORITY_2 2
#define PRIORITY_3 3

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
QueueHandle_t structQueue1 = NULL;
int freeze = 0;

#include <termios.h>

#if 0
static void prvTask_encoder(void *pvParameters)
{
    char key;
    int n;
    //BaseType_t qsentence;

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

    void send_short()
    {
        xQueueSend(structQueue1, '.',0);
    }
    void send_long()
    {
        xQueueSend(structQueue1, '-',0);
    }
    void end_letter()
    {
        
        for(int i =0; i<3; i++){//There should be an interval equal to 3x '.' between each letter
        xQueueSend(structQueue1, ' ',0);
        }
    }
    void end_sentence()
    {
        for(int i =0; i<7; i++){//There should be an interval equal to 7x '.' between each word
        xQueueSend(structQueue1, ' ',0);
        }
    }
    for (;;)
    {
       
        key = getchar();
        
        switch (key)
        {
            case 'a':
            send_short();
            send_long();
            end_letter();
            break;

            case 'b':
            send_long();
            send_short();
            send_short();
            send_short();
            end_letter();
            break;

            case 'c':
            send_long();
            send_short();
            send_long();
            send_short();
            end_letter();
            break;

            case 'd':
            send_long();
            send_short();
            send_short();
            end_letter();
            break;

            case 'e':
            send_short();
            end_letter();
            break;

            case 'f':
            send_short();
            send_short();
            send_long();
            send_short();
            end_letter();
            break;

            case 'g':
            send_long();
            send_long();
            send_short();
            end_letter();
            break;

            case 'h':
            send_short();
            send_short();
            send_short();
            send_short();
            end_letter();
            break;

            case 'i':
            send_short();
            send_short();
            end_letter();
            break;

            case 'j':
            send_short();
            send_long();
            send_long();
            send_long();
            end_letter();
            break;

            case 'k':
            send_long();
            send_short();
            send_long();
            end_letter();
            break;

            case 'l':
            send_short();
            send_long();
            send_short();
            send_short();
            end_letter();
            break;

            case 'm':
            send_long();
            send_long();
            end_letter();
            break;

            case 'n':
            send_long();
            send_short();
            end_letter();
            break;

            case 'o':
            send_long();
            send_long();
            send_long();
            end_letter();
            break;

            case 'p':
            send_short();
            send_long();
            send_long();
            send_short();
            end_letter();
            break;

            case 'q':
            send_long();
            send_long();
            send_short();
            send_long();
            end_letter();
            break;

            case 'r':
            send_short();
            send_long();
            send_short();
            end_letter();
            break;

            case 's':
            send_short();
            send_short();
            send_short();
            end_letter();
            break;

            case 't':
            send_long();
            end_letter();
            break;

            case 'u':
            send_short();
            send_short();
            send_long();
            end_letter();
            break;

            case 'v':
            send_short();
            send_short();
            send_short();
            send_long();
            end_letter();
            break;

            case 'x':
            send_long();
            send_short();
            send_short();
            send_long();
            end_letter();
            break;

            case 'y':
            send_long();
            send_short();
            send_long();
            send_long();
            end_letter();
            break;

            case 'z':
            send_long();
            send_long();
            send_short();
            send_short();
            end_letter();
            break;

            case '1':
            send_short();
            send_long();
            send_long();
            send_long();
            send_long();
            end_letter();
            break;

            case '2':
            send_short();
            send_short();
            send_long();
            send_long();
            send_long();
            end_letter();
            break;

            case '3':
            send_short();
            send_short();
            send_short();
            end_letter();
            break;

            case '4':
            send_short();
            send_short();
            send_short();
            send_short();
            send_long();
            end_letter();
            break;

            case '5':
            send_short();
            send_short();
            send_short();
            send_short();
            send_short();
            end_letter();
            break;

            case '6':
            send_long();
            send_short();
            send_short();
            send_short();
            send_short();
            end_letter();
            break;

            case '7':
            send_long();
            send_long();
            send_short();
            send_short();
            send_short();
            end_letter();
            break;

            case '8':
            send_long();
            send_long();
            send_long();
            send_short();
            send_short();
            end_letter();
            break;

            case '9':
            send_long();
            send_long();
            send_long();
            send_long();
            send_short();
            end_letter();
            break;

            case '0':
            send_long();
            send_long();
            send_long();
            send_long();
            send_long();
            end_letter();
            break;

            case ' ':
            end_sentence();
            break;

            case'\n':
            
            printf("Enter");
            break;
        }
        
       
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    tcsetattr(0, TCSANOW, &initial_settings);
    ENABLE_CURSOR();
    exit(0);
    vTaskDelete(NULL);
    
}
#endif

static void prvTask_getChar(void *pvParameters)
{
    char key;
    int n;
    // BaseType_t qsentence;

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

    void send_short()
    {
        xQueueSend(structQueue1, '.', 0); // sends info to the print queue
        xQueueSend(structQueue, '.', 0);  // sends inf to the led queue
    }
    void send_long()
    {
        xQueueSend(structQueue1, '-', 0);
        xQueueSend(structQueue, '.', 0);
    }
    void end_letter()
    {
        
            for (int i = 0; i < 3; i++)
        { // There should be an interval equal to 3x '.' between each letter
            xQueueSend(structQueue1, ' ', 0);
            xQueueSend(structQueue, '.', 0);
        }
    }
    void end_sentence()
    {
        for (int i = 0; i < 7; i++)
        { // There should be an interval equal to 7x '.' between each word
            xQueueSend(structQueue1, ' ', 0);
            xQueueSend(structQueue, '.', 0);
        }
    }
    for (;;)
    {
        // int stop = 0;
        key = getchar();

        switch (key)
        {
        case 'a':
            send_short();
            send_long();
            end_letter();
            break;

        case 'b':
            send_long();
            send_short();
            send_short();
            send_short();
            end_letter();
            break;

        case 'c':
            send_long();
            send_short();
            send_long();
            send_short();
            end_letter();
            break;

        case 'd':
            send_long();
            send_short();
            send_short();
            end_letter();
            break;

        case 'e':
            send_short();
            end_letter();
            break;

        case 'f':
            send_short();
            send_short();
            send_long();
            send_short();
            end_letter();
            break;

        case 'g':
            send_long();
            send_long();
            send_short();
            end_letter();
            break;

        case 'h':
            send_short();
            send_short();
            send_short();
            send_short();
            end_letter();
            break;

        case 'i':
            send_short();
            send_short();
            end_letter();
            break;

        case 'j':
            send_short();
            send_long();
            send_long();
            send_long();
            end_letter();
            break;

        case 'k':
            send_long();
            send_short();
            send_long();
            end_letter();
            break;

        case 'l':
            send_short();
            send_long();
            send_short();
            send_short();
            end_letter();
            break;

        case 'm':
            send_long();
            send_long();
            end_letter();
            break;

        case 'n':
            send_long();
            send_short();
            end_letter();
            break;

        case 'o':
            send_long();
            send_long();
            send_long();
            end_letter();
            break;

        case 'p':
            send_short();
            send_long();
            send_long();
            send_short();
            end_letter();
            break;

        case 'q':
            send_long();
            send_long();
            send_short();
            send_long();
            end_letter();
            break;

        case 'r':
            send_short();
            send_long();
            send_short();
            end_letter();
            break;

        case 's':
            send_short();
            send_short();
            send_short();
            end_letter();
            break;

        case 't':
            send_long();
            end_letter();
            break;

        case 'u':
            send_short();
            send_short();
            send_long();
            end_letter();
            break;

        case 'v':
            send_short();
            send_short();
            send_short();
            send_long();
            end_letter();
            break;

        case 'x':
            send_long();
            send_short();
            send_short();
            send_long();
            end_letter();
            break;

        case 'y':
            send_long();
            send_short();
            send_long();
            send_long();
            end_letter();
            break;

        case 'z':
            send_long();
            send_long();
            send_short();
            send_short();
            end_letter();
            break;

        case '1':
            send_short();
            send_long();
            send_long();
            send_long();
            send_long();
            end_letter();
            break;

        case '2':
            send_short();
            send_short();
            send_long();
            send_long();
            send_long();
            end_letter();
            break;

        case '3':
            send_short();
            send_short();
            send_short();
            send_long();
            send_long();
            end_letter();
            break;

        case '4':
            send_short();
            send_short();
            send_short();
            send_short();
            send_long();
            end_letter();
            break;

        case '5':
            send_short();
            send_short();
            send_short();
            send_short();
            send_short();
            end_letter();
            break;

        case '6':
            send_long();
            send_short();
            send_short();
            send_short();
            send_short();
            end_letter();
            break;

        case '7':
            send_long();
            send_long();
            send_short();
            send_short();
            send_short();
            end_letter();
            break;

        case '8':
            send_long();
            send_long();
            send_long();
            send_short();
            send_short();
            end_letter();
            break;

        case '9':
            send_long();
            send_long();
            send_long();
            send_long();
            send_short();
            end_letter();
            break;

        case '0':
            send_long();
            send_long();
            send_long();
            send_long();
            send_long();
            end_letter();
            break;

        case ' ':
            end_sentence();
            break;
        }
  
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
    char key;
    vTaskSuspend(redTask_hdlr);

    for (;;)
    {

        if (xQueueReceive(structQueue1, &key, portMAX_DELAY) == pdPASS)
        {

            if (key = '.')
            {
                led->period_ms = 500;
            }
            else if (key = '-')
            {
                led->period_ms = 1000;
            }
            // console_print("@");
            gotoxy(led->pos, 2);
            printf("%s⬤", led->color);
            fflush(stdout);
            vTaskDelay(led->period_ms / portTICK_PERIOD_MS);
            // vTaskDelayUntil(&xLastWakeTime, led->period_ms / portTICK_PERIOD_MS);
            if (pvParameters == &red && freeze == 1)
            {
                vTaskSuspend(redTask_hdlr);
            }

            gotoxy(led->pos, 2);
            printf("%s ", BLACK);
            fflush(stdout);
            vTaskDelay(led->period_ms / portTICK_PERIOD_MS);
            // vTaskDelayUntil(&xLastWakeTime, led->period_ms / portTICK_PERIOD_MS);
            if (pvParameters == &red)
            {
                vTaskSuspend(redTask_hdlr);
            }
            if (pvParameters == &green && freeze == 1)
            {
                vTaskSuspend(greenTask_hdlr);
            }
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
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
        if (xQueueReceive(structQueue1, &key, portMAX_DELAY) == pdPASS)
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
    structQueue = xQueueCreate(100,  // Queue length
                               1);   // Queue item size
    structQueue1 = xQueueCreate(100, // Queue length
                                1);  // Queue item size

    if (structQueue == NULL)
    {
        printf("Fail on create queue\n");
        exit(1);
    }

    if (structQueue1 == NULL)
    {
        printf("Failed to create Queue1");
        exit(1);
    }

    clear();
    DISABLE_CURSOR();
    printf(
        "╔═════════════════╗\n"
        "║                 ║\n"
        "╚═════════════════╝\n");

    xTaskCreate(prvTask_logger, "logger", configMINIMAL_STACK_SIZE, NULL, PRIORITY_1, NULL);
    // xTaskCreate(prvTask_led, "LED_green", configMINIMAL_STACK_SIZE, &green, TASK1_PRIORITY, &greenTask_hdlr);
     xTaskCreate(prvTask_led, "LED_red", configMINIMAL_STACK_SIZE, &red, PRIORITY_1, NULL);
    xTaskCreate(prvTask_getChar, "Get_key", configMINIMAL_STACK_SIZE, NULL, PRIORITY_1, NULL);
    // xTaskCreate(prvTask_text_processor, "Processor", configMINIMAL_STACK_SIZE, NULL, PRIORITY_1, NULL);

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
