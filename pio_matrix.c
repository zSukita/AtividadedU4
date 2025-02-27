#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"
#include "pico/time.h"

// Arquivo .pio gerado
#include "pio_matrix.pio.h"

#define NUM_PIXELS 25
#define OUT_PIN 7

// Botões
const uint button_0 = 5;  // Botão A (Incremento)
const uint button_1 = 6;  // Botão B (Decremento)

// LED RGB
const uint LED_R = 13;
const uint LED_G = 12;
const uint LED_B = 11;

// Variáveis globais
volatile int numero_atual = 0;  // Número atual exibido na matriz
volatile uint32_t ultimo_tempo_botao = 0;  // Último tempo em que um botão foi pressionado
const uint32_t debounce_delay = 200;  // Tempo de debounce para evitar múltiplas leituras de um botão


// Matrizes que representam os dígitos de 0 a 9
const uint32_t digitos[10][25] = {
    // 0 (Corrigido com orientação física)
    {1,1,1,1,1,
     1,0,0,0,1,
     1,0,0,0,1,
     1,0,0,0,1,
     1,1,1,1,1},
    
    // 1
    {0,0,1,0,0,
     0,1,1,0,0,
     0,0,1,0,0,
     0,0,1,0,0,
     0,1,1,1,0},
    
    // 2
    {1,1,1,1,1,
     0,0,0,0,1,
     1,1,1,1,1,
     1,0,0,0,0,
     1,1,1,1,1},
    
    // 3
    {1,1,1,1,1,
     0,0,0,0,1,
     1,1,1,1,1,
     0,0,0,0,1,
     1,1,1,1,1},
    
    // 4
    {1,0,0,0,1,
     1,0,0,0,1,
     1,1,1,1,1,
     0,0,0,0,1,
     1,0,0,0,0},
    
    // 5
    {1,1,1,1,1,
     1,0,0,0,0,
     1,1,1,1,1,
     0,0,0,0,1,
     1,1,1,1,1},
    
    // 6
    {1,1,1,1,1,
     1,0,0,0,0,
     1,1,1,1,1,
     1,0,0,0,1,
     1,1,1,1,1},
    
    // 7
    {1,1,1,1,1,
     0,0,0,0,1,
     0,1,0,0,0,
     0,0,1,0,0,
     0,0,0,1,0},
    
    // 8
    {1,1,1,1,1,
     1,0,0,0,1,
     1,1,1,1,1,
     1,0,0,0,1,
     1,1,1,1,1},
    
    // 9
    {1,1,1,1,1,
     1,0,0,0,1,
     1,1,1,1,1,
     0,0,0,0,1,
     1,1,1,1,1}
};
// Função para corrigir a posição dos LEDs na matriz
int posicao_corrigida(int x, int y) {
    return (4 - y) * 5 + x;  // Inverte ordem das linhas
}

// Função de interrupção para tratamento dos botões
void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());
    
    // Verifica se o tempo desde a última interrupção é maior que o debounce_delay
    if(tempo_atual - ultimo_tempo_botao > debounce_delay) {
        if(gpio == button_0) {
            numero_atual = (numero_atual + 1) % 10;
        } 
        else if(gpio == button_1) {
            numero_atual = (numero_atual - 1 + 10) % 10;
        }
        ultimo_tempo_botao = tempo_atual;
    }
}

// Função de callback para o timer que controla o LED RGB
bool timer_handler(repeating_timer_t *rt) {
    static bool led_state = false;
    led_state = !led_state;
    gpio_put(LED_R, led_state);
    return true;
}

// Função para atualizar a matriz de LEDs com o dígito atual
void atualiza_matriz(const uint32_t digito[25], PIO pio, uint sm) {
    // Define um brilho fixo (por exemplo, 50% do máximo)
    const uint8_t brightness = 50; // Pode ajustar este valor entre 0-255
    
    // Percorre a matriz 5x5 e atualiza o estado de cada LED
    for(int y = 0; y < 5; y++) {
        for(int x = 0; x < 5; x++) {
            uint32_t index = posicao_corrigida(x, y);
            // Aplica o brilho ao LED (vermelho com brilho reduzido ou desligado)
            uint32_t cor = digito[index] ? 
                          ((uint32_t)brightness << 16) : // Vermelho com brilho reduzido
                          0x00000000;                    // Desligado
            pio_sm_put_blocking(pio, sm, cor << 8u);
        }
    }
}

// Função principal
int main() {
    stdio_init_all(); // Inicializa a comunicação serial
    
    // Inicialização do PIO e configuração dos pinos
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &pio_matrix_program);
    uint sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, OUT_PIN);

    // Configuração dos botões
    gpio_init(button_0);
    gpio_set_dir(button_0, GPIO_IN);
    gpio_pull_up(button_0);
    
    gpio_init(button_1);
    gpio_set_dir(button_1, GPIO_IN);
    gpio_pull_up(button_1);

    // Configuração das interrupções dos botões
    gpio_set_irq_enabled_with_callback(button_0, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(button_1, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // Configuração dos LEDs RGB
    gpio_init(LED_R);
    gpio_set_dir(LED_R, GPIO_OUT);
    gpio_put(LED_G, false);
    gpio_put(LED_B, false);

    // Configuração do timer para controle do LED RGB
    repeating_timer_t timer;
    add_repeating_timer_ms(100, timer_handler, NULL, &timer);

     // Loop principal que atualiza a matriz de LEDs com o número atual
    while(true) {
        atualiza_matriz(digitos[numero_atual], pio, sm);
        sleep_ms(10);
    }
}