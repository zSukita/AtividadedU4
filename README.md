# LED Matrix Display - Raspberry Pi Pico

Este projeto implementa um display de matriz de LEDs 5x5 utilizando um Raspberry Pi Pico e a linguagem C. O código permite a exibição de números de 0 a 9 e sua alteração através de botões físicos.

## 📌 Funcionalidades
- Exibição de números de 0 a 9 em uma matriz de LEDs 5x5.
- Incremento e decremento dos números via botões.
- Controle de brilho e manipulação de LEDs RGB.
- Tratamento de debounce para evitar múltiplas leituras dos botões.

## 🛠️ Requisitos
- Raspberry Pi Pico
- Matriz de LEDs 5x5 compatível
- Dois botões físicos
- LEDs RGB
- Fonte de alimentação
- SDK do Raspberry Pi Pico instalado

## 🔧 Configuração de Hardware
### Conexões:
| Componente | Pino Pico |
|------------|----------|
| Matriz LED | GPIO7 |
| Botão A (Incremento) | GPIO5 |
| Botão B (Decremento) | GPIO6 |
| LED Vermelho | GPIO13 |
| LED Verde | GPIO12 |
| LED Azul | GPIO11 |

## 🚀 Como Compilar e Executar
1. Instale o [SDK do Raspberry Pi Pico](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf).
2. Compile o código utilizando o CMake e o SDK.
3. Carregue o arquivo `.uf2` gerado no Raspberry Pi Pico.

## 📜 Código Explicado
- **Interrupções de GPIO**: Usadas para detectar pressionamentos dos botões e alterar o número exibido.
- **Matriz de LEDs**: Atualizada a cada ciclo para exibir o número correto.
- **Timer Repetitivo**: Pisca o LED vermelho para indicar funcionamento.

## 🔍 Melhorias Futuras
- Implementação de caracteres alfabéticos.
- Ajuste dinâmico de brilho via ADC.
- Integração com comunicação serial para controle remoto.

---
Desenvolvido para projetos educacionais e experimentação com hardware embarcado.

