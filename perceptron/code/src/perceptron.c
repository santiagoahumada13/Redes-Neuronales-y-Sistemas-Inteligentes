#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"



float x[] = {0.2, 0.5, 0.6};
float w[] = {0.3, 0.1, 0.7};
float bias = -0.8;

void pico_setup()
{
    #if !defined(i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
    #warning i2c / SSD1306_i2d example requires a board with I2C pins
        puts("Default I2C pins were not defined");
    #else
        bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
        bi_decl(bi_program_description("SSD1306 OLED driver I2C example for the Raspberry Pi Pico"));
        i2c_init(i2c_default, SSD1306_I2C_CLK * 1000);
        gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
        gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
        gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
        gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
}

int step(float weighted_sum)
{
    y = weighted_sum - bias;
    if(y<=0)
    {
        return 0;
    } 
    else
    {
        return 1;
    }
}

int perceptron(float x_input[], w_weight[])
{
    float weighted_sum = 0;
    int n = sizeof(x_input) / sizeof(x_input[0]);
    for(int i = 0; i< n; i++)
    {
        weighted_sum += x_input[i]*w_weight[i];
    }
    return step(weighted_sum);
}




int main()
{
    pico_setup();
    int output = perceptron();

}