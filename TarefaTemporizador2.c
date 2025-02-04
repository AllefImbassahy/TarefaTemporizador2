#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"

#define LED_RED 11
#define LED_BLUE 12
#define LED_GREEN 13
#define BUTTON 5

volatile int state = 0;
volatile bool button_pressed = false;

bool turn_off_callback(alarm_id_t id, void *user_data) {
    switch (state) {
        case 0:  // Estado inicial: todos os LEDs ligados
            gpio_put(LED_BLUE, 1);
            gpio_put(LED_RED, 1);
            gpio_put(LED_GREEN, 1);
            state++;
            break;
        case 1:  // Após 3 segundos: dois LEDs ligados
            gpio_put(LED_GREEN, 0);
            state++;
            break;
        case 2:  // Após mais 3 segundos: apenas um LED ligado
            gpio_put(LED_BLUE, 0);
            state++;
            break;
        case 3:  // Final: todos os LEDs desligados, botão pode ser pressionado novamente
            gpio_put(LED_RED, 0);
            state = 0;
            button_pressed = false;
            break;
    }
    return 0;
}

void handle_button_press() {
    if (!button_pressed) {
        button_pressed = true;
        add_alarm_in_ms(0, turn_off_callback, NULL, true);  // Inicia a sequência imediatamente
        add_alarm_in_ms(3000, turn_off_callback, NULL, true);  // Primeiro atraso de 3 segundos
        add_alarm_in_ms(6000, turn_off_callback, NULL, true);  // Segundo atraso de 3 segundos
        add_alarm_in_ms(9000, turn_off_callback, NULL, true);  // Terceiro atraso de 3 segundos
    }
}

int main() {
    stdio_init_all();

    // Configuração dos pinos como saída
    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);

    // Configuração do botão como entrada com pull-up interno
    gpio_init(BUTTON);
    gpio_set_dir(BUTTON, GPIO_IN);
    gpio_pull_up(BUTTON);

    while (1) {
        if (gpio_get(BUTTON) == 0) {  // Verifica se o botão foi pressionado
            sleep_ms(50);  // Aplica o debounce diretamente
            if (gpio_get(BUTTON) == 0) {  // Verifica novamente após o atraso
                handle_button_press();
            }
        }    
        sleep_ms(100);  // Aguarda um pouco para evitar múltiplas leituras
    }
}
