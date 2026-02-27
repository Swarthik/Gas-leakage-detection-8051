#include <reg51.h>

#define LCD_DATA  P1
#define GAS_THRESHOLD 100

sbit LCD_RS = P2^0;
sbit LCD_RW = P2^1;
sbit LCD_EN = P2^2;

sbit ADC_ALE   = P2^4;
sbit ADC_START = P2^5;
sbit ADC_EOC   = P2^6;
sbit ADC_OE    = P2^7;

sbit BUZZER = P3^7;
sbit LED    = P3^6;

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 123; j++);
}

void delay_us(unsigned int us)
{
    unsigned int i;
    for (i = 0; i < us; i++);
}

void lcd_enable_pulse(void)
{
    LCD_EN = 1;
    delay_us(5);
    LCD_EN = 0;
    delay_us(5);
}

void lcd_send_cmd(unsigned char cmd)
{
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_DATA = cmd;
    lcd_enable_pulse();
    delay_ms(2);
}

void lcd_send_data(unsigned char dat)
{
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_DATA = dat;
    lcd_enable_pulse();
    delay_us(50);
}

void lcd_init(void)
{
    delay_ms(20);
    lcd_send_cmd(0x38);
    lcd_send_cmd(0x0C);
    lcd_send_cmd(0x06);
    lcd_send_cmd(0x01);
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

void lcd_print_num(unsigned int num)
{
    lcd_send_data('0' + num / 100);
    lcd_send_data('0' + (num % 100) / 10);
    lcd_send_data('0' + num % 10);
}

unsigned char adc_read(void)
{
    unsigned char result;

    ADC_ALE = 1;
    delay_us(2);
    ADC_ALE = 0;

    ADC_START = 1;
    delay_us(2);
    ADC_START = 0;

    while (ADC_EOC == 1);
    while (ADC_EOC == 0);

    ADC_OE = 1;
    delay_us(2);
    result = P0;
    ADC_OE = 0;

    return result;
}

void main(void)
{
    unsigned char gas_value;

    BUZZER = 0;
    LED    = 0;
    ADC_ALE = ADC_START = ADC_OE = 0;
    P0 = 0xFF;

    lcd_init();

    lcd_set_cursor(0, 0);
    lcd_print(" GAS LEAK SYSTEM");
    lcd_set_cursor(1, 0);
    lcd_print("  Initialising..");
    delay_ms(2000);
    lcd_send_cmd(0x01);
    delay_ms(5);

    while (1)
    {
        gas_value = adc_read();

        lcd_set_cursor(0, 0);
        lcd_print("Gas Level: ");
        lcd_print_num(gas_value);

        if (gas_value > GAS_THRESHOLD)
        {
            lcd_set_cursor(1, 0);
            lcd_print("** GAS DETECTED!**");
            BUZZER = 1;
            LED    = 1;
        }
        else
        {
            lcd_set_cursor(1, 0);
            lcd_print("Status: SAFE    ");
            BUZZER = 0;
            LED    = 0;
        }

        delay_ms(500);
    }
}
