//
// Sample program that scans input from serial port, for example
// interactive sessions with a host computer running a terminal
// program, and positions a servo accordingly.
//

#include <errno.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#define BAUD_RATE 9600
#define BIT_RATE_IN_TIMER_TICKS ((F_CPU / BAUD_RATE / 16) - 1)
#define NOINLINE __attribute__((noinline))

#define putstring(s) putstring_P(PSTR(s))
#define putline(s) putstring_P(PSTR(s "\r\n"))

static FILE uartout = {0};
static FILE uartin = {0};

static NOINLINE void putstring_P(PGM_P str)
    {
    for (uint8_t c; (c = pgm_read_byte(str)); str++)
        putchar(c);
    }

void stall(double milliseconds)
    {
    _delay_ms(milliseconds);
    } 

void PrepareBlinker()
    {
    DDRB = 0xFF; // Set PORTB for output
    }

void FlashBlinker(double durationMilliseconds)
    {
    PORTB = 0xFF;
    stall(durationMilliseconds);
    PORTB = 0x00;
    }

void PrepareTerminal(unsigned int bittimer)
    {
    // Specify baud
    UBRR0H = (unsigned char) (bittimer >> 8);
    UBRR0L = (unsigned char) bittimer;
    // Specify data, stop bits (8N1)
    UCSR0C = (3 << UCSZ00);
    // Make it so
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    }

void WriteToTerminal(unsigned char c)
    {
    while (!(UCSR0A & (1 << UDRE0))); // Busy wait that tests if the USART Data Register is Empty
    UDR0 = c;
    }

void ReadFromTerminal(unsigned char* c)
    {
    while (!(UCSR0A & (1 << RXC0))); // Busy wait that tests if the USART Receive Complete is Set
    (*c) = UDR0;
    }

static int PutCharacterStandardOutput(char c, FILE* stream)
    {
    WriteToTerminal(c);
    return 0;
    }

int GetCharacterStandardInput(FILE* stream) 
    {
    unsigned char ch;
    ReadFromTerminal(&ch);
    return ch;
    }

void PrepareStandardOutput(void)
    {
    fdev_setup_stream(&uartout, PutCharacterStandardOutput, NULL, _FDEV_SETUP_WRITE);
    stdout = &uartout;
    }

void PrepareStandardInput(void)
    {
    fdev_setup_stream(&uartin, NULL, GetCharacterStandardInput, _FDEV_SETUP_READ);
    stdin = &uartin;
    }

uint8_t InputAvailable(void)
    {
    return UCSR0A & _BV(RXC0); // Tests if the USART Receive Complete is Set
    }

void ProcessEventsIndefinitely(void)
    {
    while (1);
    }

static uint32_t CalculateRamFree(void)
    {
    extern int  __bss_end;
    extern int* __brkval;
    int freeMemory;
    
    if (reinterpret_cast<int>(__brkval) == 0)
        {
        // if no heap use from end of bss section
        freeMemory = reinterpret_cast<int>(&freeMemory) - reinterpret_cast<int>(&__bss_end);
        }
    else
        {
        // use from top of stack to heap
        freeMemory = reinterpret_cast<int>(&freeMemory) - reinterpret_cast<int>(__brkval);
        }
    
    return (uint32_t) freeMemory;
    }

void PrintFreeRam(FILE* f)
    {
    fprintf(f, "Free RAM: %u bytes\r\n", CalculateRamFree());
    }

int main(void)
    {
    PrepareBlinker();
    PrepareTerminal(BIT_RATE_IN_TIMER_TICKS);
    PrepareStandardOutput();
    PrepareStandardInput();
    PrintFreeRam(stdout);

    TCCR0B = 0;                  // Stop clock of timer/counter 0
    DDRD = _BV(DDD6);            // Configure port D, pin 6 for output
    PORTD &= (uint8_t) (~_BV(PIND6)); // Set output to '0' (effective?)

    putstring("\r\n");
    putstring("Keyboard controls\r\n");
    putstring("-----------------\r\n\r\n");
    putstring("    ENTER  Reset\r\n");
    putstring("    SPACE  Move to center\r\n");
    putstring("    <      Set counter-clockwise limit\r\n");
    putstring("    >      Set clockwise limit\r\n");
    putstring("    ,      Move counter-clockwise\r\n");
    putstring("    .      Move clockwise\r\n");
    putstring("    0-9    Move to relative position between min and max.\r\n");
    putstring("\r\n");

    uint16_t servoPulseDefault = 1500;                 // microseconds
    uint16_t servoPulseWidthMin = 0;                   // microseconds
    uint16_t servoPulseWidthMax = UINT16_MAX;          // microseconds
    uint16_t servoPulseWidth = servoPulseDefault;      // microseconds
    uint16_t servoPulseIncrement = 50;                 // microseconds
    uint16_t servoPulseOverhead = 500;                 // microseconds
    uint16_t servoPulseDelay = servoPulseWidth - servoPulseOverhead;
    
    while (1) 
        {
        stall(20);         // Servos allegedly require 20 ms refresh
        
        if (InputAvailable())
            {
            // Read a character and echo the corresponding servo angle.
            int16_t c = fgetc(stdin);
            if (c == EOF)
                {
                if (feof(stdin))
                    printf("\r\nEncountered end of file.\r\n");
                if (ferror(stdin))
                    printf("\r\nEncountered error number %d.\r\n", errno);
                ProcessEventsIndefinitely();
                }

            if (c == ',')
                {
                if (servoPulseWidth > servoPulseIncrement)
                    servoPulseWidth -= servoPulseIncrement;
                if (servoPulseWidth < servoPulseWidthMin)
                    servoPulseWidth = servoPulseWidthMin;
                printf("Positioning servo with %u microsecond pulse.\r\n", servoPulseWidth);
                }
            else if (c == '.')
                {
                if ((UINT16_MAX - servoPulseWidth) > servoPulseIncrement)                    
                    servoPulseWidth += servoPulseIncrement;
                if (servoPulseWidth > servoPulseWidthMax)
                    servoPulseWidth = servoPulseWidthMax;
                printf("Positioning servo with %u microsecond pulse.\r\n", servoPulseWidth);
                }
            else if (c == ' ')
                {
                servoPulseWidth = lrint((servoPulseWidthMin + servoPulseWidthMax) / 2.0);
                printf("Centering servo with %u microsecond pulse in [%u, %u].\r\n",
                    servoPulseWidth,
                    servoPulseWidthMin,
                    servoPulseWidthMax);
                }
            else if (c == '\r')
                {
                servoPulseWidthMin = 0;                // microseconds
                servoPulseWidthMax = UINT16_MAX;       // microseconds
                servoPulseWidth = servoPulseDefault;   // microseconds
                printf("Resetting servo with %u microsecond pulse in [%u, %u].\r\n",
                    servoPulseWidth,
                    servoPulseWidthMin,
                    servoPulseWidthMax);
                }
            else if (c == '<')
                {
                servoPulseWidthMin = servoPulseWidth;
                printf("Minimum pulse width set to %u microseconds.\r\n", servoPulseWidthMin);
                }
            else if (c == '>')
                {
                servoPulseWidthMax = servoPulseWidth;
                printf("Maximum pulse width set to %u microseconds.\r\n", servoPulseWidthMax);
                }
            else if (c >= '0' && c <= '9')
                {
                uint16_t servoPosition = c - '0';
                servoPulseWidth = lrint( (double) servoPulseWidthMin + (double) servoPosition * (servoPulseWidthMax - servoPulseWidthMin) / 9.0 );
                printf("Positioning servo at position %u with %u microsecond pulse in [%u, %u].\r\n",
                    servoPosition,
                    servoPulseWidth,
                    servoPulseWidthMin,
                    servoPulseWidthMax);
                }
            else
                {
                printf("Invalid command: '%c'\r\n", c);
                }

            if (servoPulseWidth > servoPulseOverhead)
                servoPulseDelay = servoPulseWidth - servoPulseOverhead;
            else
                servoPulseDelay = 0;
            
            }


        // Pulse the servo
        PORTD |= (uint8_t) (_BV(PIND6));               // ON
        _delay_us(servoPulseDelay);                    // ... wait ...
        PORTD &= (uint8_t) (~_BV(PIND6));              // OFF
        }

    return 0;
    }

