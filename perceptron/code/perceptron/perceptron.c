#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#define GPIO_PIR    21
#define ADC_GPIO    26
#define ADC_NUM     0

const float delta = 0.3;
int epoch = 0;

int wifi_init()
{
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }
}

uint8_t step(float in_sum, float bias)
{
    float step_val = in_sum - bias;  //Resta el bias a la sumatoria
    if(step_val<=0) //Si es igual o menor que 0 la salida es 0
    {
        return 0;
    }else{          //Si es mayor que 0 la salida es 1
        return 1;
    }
}

uint8_t perceptron(float x_input, float y_input, float w_weight[], float bias)
{
    float weighted_sum = 0;      //La sumatoria inicial es cero
    weighted_sum += x_input*w_weight[0] + y_input*w_weight[1]; //Sumatoria de los pesos por las entradas.
    return step(weighted_sum,bias); //Llamado a la función de activación

}

bool training (float x_input_arr[], float y_input_arr[], float w_weight[], float* bias, uint8_t expected_result[], int array_size)
{
    bool no_error = true;
    for(int j=0; j<array_size; j++)
    {
        uint8_t obtained = perceptron(x_input_arr[j], y_input_arr[j], w_weight, *bias);
        if(obtained!=expected_result[j]) 
        {
            no_error=false;
            int e = expected_result[j] - obtained;
            *bias += (-e)*(delta);
            w_weight[0] += e * delta * x_input_arr[j];
            w_weight[1] += e * delta * y_input_arr[j];
        }
    }
    return no_error;
}


int main()
{
    stdio_init_all();
    //Configuración de puertos
    gpio_init(GPIO_PIR);
    gpio_pull_down(GPIO_PIR);
    gpio_set_dir(GPIO_PIR,GPIO_IN);

    //Configuracion de la conversión análogico-digital
    adc_init();
    adc_gpio_init(ADC_GPIO);
    adc_select_input(ADC_NUM);
    const float conv_fact= 3.3f/(1<<12);
    
    wifi_init(); //Se inicializa el modulo wifi porque es el que tiene el control del LED interno de la tarjeta
    
    //Instrucción de inicio del programa
    int start = 0;
    sleep_ms(10000);
    printf("Presione 1 para iniciar\n");
    scanf("%d",&start);
    float w_training[2];
    if(start==1)
    {
        printf("En este momento se generará el entrenamiento del perceptrón\n");

        float x_training[]={0.8, 1.8, 2, 2.7, 3.3, 2.2, 3};
        float y_training[]={0,1,0,0,1,1,0};
        uint8_t expected_result[]={0,0,0,0,1,1,0};
        w_training[0]= 0.747;
        w_training[1]= 0.284;

        int array_size = sizeof(x_training) / sizeof(x_training[0]);


        printf("Pesos Iniciales: \n");
        printf("w1 = %f\n",w_training[0]);
        printf("w2= %f\n",w_training[1]);
        float bias = 0.124;
        printf("Bias Inicial: %f\n", bias);
        
        for (int i=0;i<array_size;i++) {
            x_training[i] /= 3.3; 
            printf("%f\n",x_training[i]);
        }
    
        while(training(x_training,y_training,w_training,&bias,expected_result,array_size)==false)
        {
            epoch++;
            printf("Época %d: w1=%.3f w2=%.3f bias=%.3f\n", epoch, w_training[0], w_training[1], bias);
            if(epoch>1000)
            {
                printf("ERROR: El entrenamiento no termino despues de 1000 epocas");
                break;
            }
        }
        while(true)
        {
            float luz_in = adc_read() * conv_fact;
            luz_in /= 3.3;
            sleep_ms(1000);
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, perceptron(luz_in, gpio_get(GPIO_PIR), w_training, bias));
            printf("%f\n",luz_in);
            if(gpio_get(GPIO_PIR))
            {
                printf("Movimiento Detectado\n");
                sleep_ms(2000);
            }else{
                printf("Movimiento no detectado\n");
                sleep_ms(2000);
            }
            
        }
        
    }else{
        printf("Tecla invalida intente nuevamente");
    }

}
