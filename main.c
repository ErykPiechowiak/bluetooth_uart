#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <string.h>

//#define uart0 ((uart_inst_t * const)uart0_hw)
//#define uart1 ((uart_inst_t * const)uart1_hw)

#define UART_ID uart0
#define BAUD_RATE 9600
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE
#define UART0_IRQ 20
#define LED_PIN 22

void check_valid_command(const char *received_string, size_t string_length){

    //Turn on the diode
    if(strcmp(received_string,"Diode ON") == 0){
        gpio_put(LED_PIN,1);
        if(uart_is_writable(UART_ID))
            uart_puts(UART_ID,"Diode ON!");
    }
    //Turn off the diode
    else if(strcmp(received_string,"Diode OFF") == 0){
        gpio_put(LED_PIN,0);
        if(uart_is_writable(UART_ID))
            uart_puts(UART_ID,"Diode OFF!");
    }
    //Not handled command
    else{
        if(uart_is_writable(UART_ID))
            uart_puts(UART_ID,"Not valid command!");
    }
}

// RX interrupt handler
void on_uart_rx() {
    char received_string[100]; 
    uint8_t received_char;
    size_t chars_received = 0; 
    //Read whole buffer
    while (uart_is_readable_within_us(UART_ID,10000)) {
        received_string[chars_received] = uart_getc(UART_ID);
        chars_received++;
    }
    //add null termination char
    received_string[chars_received] = '\0';
    //if(uart_is_writable(UART_ID))
    //    uart_puts(UART_ID,received_string);

    check_valid_command(received_string, chars_received);

}



int main()
{
    uart_init(UART_ID, 9600);
    gpio_set_function(0,GPIO_FUNC_UART);
    gpio_set_function(1,GPIO_FUNC_UART);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN,GPIO_OUT);

    // The call will return the actual baud rate selected, which will be as close as
    // possible to that requested
    int __unused actual = uart_set_baudrate(UART_ID, BAUD_RATE);

    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow(UART_ID, false, false);

    // Set our data format
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);

    // Turn off FIFO's - we want to do this character by character
    uart_set_fifo_enabled(UART_ID, false);

    // Set up a RX interrupt
    // We need to set up the handler first
    // Select correct interrupt for the UART we are using
    int UART_IRQ = UART0_IRQ;

    // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    irq_set_enabled(UART_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(UART_ID, true, false);

    // OK, all set up.
    // Lets send a basic string out, and then run a loop and wait for RX interrupts
    // The handler will count them, but also reflect the incoming data back with a slight change!
    //uart_puts(UART_ID, "AT+BAUD4");
    //uart_puts(UART_ID, "AT+BAUD4");
    //uart_puts(UART_ID, "AT+BAUD4");


    while (1)
        tight_loop_contents();

/*
    int blink = 1;
    while(true)
    {
        gpio_put(led_pin,blink);
        blink == 1 ? blink = 0 : blink = 1;
        uart_puts(uart0,"Hello! Hello!\n");
        sleep_ms(1000);

    }
*/
    return 0;
}