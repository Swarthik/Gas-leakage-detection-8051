/*
 * Gas Leakage Detection System
 * Microcontroller : AT89C51 (8051)
 * Sensor          : MQ-2 Gas Sensor (via ADC0808)
 * Display         : 16x2 LCD (Port 1)
 * Buzzer          : P3.7
 * LED             : P3.6
 * ADC0808 Data    : Port 0 (P0.0 - P0.7)
 * ADC Control     : ALE -> P2.4 | START -> P2.5 | EOC -> P2.6 | OE -> P2.7
 *
 * Compiled with   : Keil uVision (C51 compiler)
 */

#include <reg51.h>

/* ─────────────── LCD Pin Definitions ─────────────── */
#define LCD_DATA  P1          /* D0-D7 of LCD on Port 1 */

sbit LCD_RS = P2^0;           /* Register Select        */
sbit LCD_RW = P2^1;           /* Read / Write           */
sbit LCD_EN = P2^2;           /* Enable                 */

/* ─────────────── ADC0808 Control Pins ─────────────── */
sbit ADC_ALE   = P2^4;        /* Address Latch Enable   */
sbit ADC_START = P2^5;        /* Start Conversion       */
sbit ADC_EOC   = P2^6;        /* End of Conversion      */
sbit ADC_OE    = P2^7;        /* Output Enable          */

/* ─────────────── Alert Output Pins ─────────────────── */
sbit BUZZER = P3^7;
sbit LED    = P3^6;

/* ─────────────── Threshold ──────────────────────────── */
#define GAS_THRESHOLD  100    /* 0-255 ADC scale; tune as needed */

/* ══════════════════════════════════════════════════════
   DELAY FUNCTIONS
   ══════════════════════════════════════════════════════ */
void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 123; j++);   /* ~1 ms at 11.0592 MHz */
}

void delay_us(unsigned int us)
{
    unsigned int i;
    for (i = 0; i < us; i++);
}

/* ══════════════════════════════════════════════════════
   LCD FUNCTIONS
   ══════════════════════════════════════════════════════ */
void lcd_enable_pulse(void)
{
    LCD_EN = 1;
    delay_us(5);
    LCD_EN = 0;
    delay_us(5);
}

void lcd_send_cmd(unsigned char cmd)
{
    LCD_RS = 0;               /* Command mode  */
    LCD_RW = 0;               /* Write mode    */
    LCD_DATA = cmd;
    lcd_enable_pulse();
    delay_ms(2);
}

void lcd_send_data(unsigned char dat)
{
    LCD_RS = 1;               /* Data mode     */
    LCD_RW = 0;               /* Write mode    */
    LCD_DATA = dat;
    lcd_enable_pulse();
    delay_us(50);
}

void lcd_init(void)
{
    delay_ms(20);
    lcd_send_cmd(0x38);       /* 8-bit, 2-line, 5×7 font */
    lcd_send_cmd(0x0C);       /* Display ON, cursor OFF  */
    lcd_send_cmd(0x06);       /* Entry mode: increment   */
    lcd_send_cmd(0x01);       /* Clear display           */
    delay_ms(2);
}

void lcd_set_cursor(unsigned char row, unsigned char col)
{
    unsigned char pos;
    pos = (row == 0) ? (0x80 + col) : (0xC0 + col);
    lcd_send_cmd(pos);
}

void lcd_print(const char *str)
{
    while (*str)
        lcd_send_data(*str++);
}

/* Print a 3-digit unsigned integer on LCD */
void lcd_print_num(unsigned int num)
{
    unsigned char hundreds, tens, ones;
    hundreds = num / 100;
    tens     = (num % 100) / 10;
    ones     = num % 10;
    lcd_send_data('0' + hundreds);
    lcd_send_data('0' + tens);
    lcd_send_data('0' + ones);
}

/* ══════════════════════════════════════════════════════
   ADC0808 FUNCTIONS
   (Channel 0 — IN0 connected to MQ-2 analog output)
   ══════════════════════════════════════════════════════ */
unsigned char adc_read(void)
{
    unsigned char result;

    /* Select Channel 0: ADDA=0, ADDB=0, ADDC=0 (already wired or
       driven by lower port bits — adjust if your wiring differs) */

    ADC_ALE   = 1;            /* Latch channel address   */
    delay_us(2);
    ADC_ALE   = 0;

    ADC_START = 1;            /* Pulse START high        */
    delay_us(2);
    ADC_START = 0;            /* Start conversion        */

    /* Wait for EOC to go LOW then HIGH (conversion done) */
    while (ADC_EOC == 1);     /* Wait for EOC to fall    */
    while (ADC_EOC == 0);     /* Wait for EOC to rise    */

    ADC_OE = 1;               /* Enable output           */
    delay_us(2);
    result = P0;              /* Read 8-bit digital value */
    ADC_OE = 0;               /* Disable output          */

    return result;
}

/* ══════════════════════════════════════════════════════
   ALERT FUNCTIONS
   ══════════════════════════════════════════════════════ */
void alert_on(void)
{
    BUZZER = 1;
    LED    = 1;
}

void alert_off(void)
{
    BUZZER = 0;
    LED    = 0;
}

/* ══════════════════════════════════════════════════════
   MAIN
   ══════════════════════════════════════════════════════ */
void main(void)
{
    unsigned char gas_value;

    /* ── Initialise outputs ── */
    BUZZER = 0;
    LED    = 0;

    /* ── ADC control lines idle state ── */
    ADC_ALE   = 0;
    ADC_START = 0;
    ADC_OE    = 0;

    /* ── Port 0 as input for ADC data ── */
    P0 = 0xFF;

    /* ── Initialise LCD ── */
    lcd_init();

    /* ── Startup message ── */
    lcd_set_cursor(0, 0);
    lcd_print(" GAS LEAK SYSTEM");
    lcd_set_cursor(1, 0);
    lcd_print("  Initialising..");
    delay_ms(2000);
    lcd_send_cmd(0x01);       /* Clear */
    delay_ms(5);

    /* ══ Main loop ══ */
    while (1)
    {
        gas_value = adc_read();

        /* ── Row 1: always show ADC value ── */
        lcd_set_cursor(0, 0);
        lcd_print("Gas Level: ");
        lcd_print_num(gas_value);

        if (gas_value > GAS_THRESHOLD)
        {
            /* ── LEAK DETECTED ── */
            lcd_set_cursor(1, 0);
            lcd_print("** GAS DETECTED!**");
            alert_on();
        }
        else
        {
            /* ── SAFE ── */
            lcd_set_cursor(1, 0);
            lcd_print("Status: SAFE    ");
            alert_off();
        }

        delay_ms(500);        /* Sampling interval ~500 ms */
    }
}
