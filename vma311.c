#include "vma311.h"
#include "freertos/FreeRTOS.h" //to run freertos on esp32
#include "freertos/task.h"//provide the multitaksing functionality

static vma311_t vma311; //creates a a static instance of a vma311 structure

static void vma311_send_start_signal();
static int vma311_wait(uint16_t, int);
static int vma311_check_response();
static inline vma311_status_t vma311_read_byte(uint8_t *);
static vma311_status_t vma311_check_crc(uint8_t *);

void vma311_init(gpio_num_t num) // gpio initialization
{
    vma311.num = num; //gives the pin value to the vma311 object's pin attribute
    vma311.last_read_time = -2000000;
    gpio_reset_pin(num);
    vTaskDelay(pdMS_TO_TICKS(1000)); //waits 1 sec before sending instruction
}

vma311_data_t vma311_get_values()
{
    vma311_data_t error_data = {VMA311_TIMEOUT_ERROR, -1, -1, -1, -1}; //in case the time for sending data is too long, the value of the bytes is -1
    uint8_t data[5] = {0, 0, 0, 0, 0};                                //initialize an array to store 5 bytes sent by the sensor
    if (esp_timer_get_time() - 2000000 < vma311.last_read_time)//if the duration since the last reading is under 2000000 ticks, the program returns the measurement
    {
        return vma311.data;
    }
    vma311.last_read_time = esp_timer_get_time();//otherwise we continue and we set the time of the last reading to the beginning of this reading
    vma311_send_start_signal();//this function "wakes" the sensor up so it initializes and sends data
    if (vma311_check_response() == VMA311_TIMEOUT_ERROR)// if the communication is too long
    {
        return error_data;
    }
    for (int i = 0; i < 5; ++i)
    {
        if (vma311_read_byte(&data[i]))//this function converts the bits sent by the sensor to a digital value
        {
            return error_data;
        }
    }
    if (vma311_check_crc(data) != VMA311_CRC_ERROR)// if there's no transmission error, we assign them to the sensor object
    {
        vma311.data.rh_int = data[0];
        vma311.data.rh_dec = data[1];
        vma311.data.t_int = data[2];
        vma311.data.t_dec = data[3];
        vma311.data.status = VMA311_OK;
    }
    return vma311.data;// temperature and humidity
}

void vma311_send_start_signal()// sending a start signal to the sensor
{
    gpio_set_direction(vma311.num, GPIO_MODE_OUTPUT);
    gpio_set_level(vma311.num, 0);
    ets_delay_us(20 * 1000);//set the gpio to 0 for at least 18 ms 
    gpio_set_level(vma311.num, 1);
    ets_delay_us(40);//set the gpio to 1 for max 40 us
}

int vma311_wait(uint16_t us, int level)
{
    int us_ticks = 0;
    while (gpio_get_level(vma311.num) == level) //level of the pin is equal to the level in parameter
    {
        if (us_ticks++ > us) // we increment us_tick at every us, if it's bigger than the delay in parameter, there's a timeout
        {
            return VMA311_TIMEOUT_ERROR; 
        }
        ets_delay_us(1);
    }
    return us_ticks;
}

int vma311_check_response()
{
    gpio_set_direction(vma311.num, GPIO_MODE_INPUT);
    //the response from the sensor normally begins with a low voltage level on the pin for 80 us
    if (vma311_wait(81, 0) == VMA311_TIMEOUT_ERROR) // if it stays on 0 for loner
    {
        return VMA311_TIMEOUT_ERROR;
    }
    //then the sensor sets the level on the pin to 1 for 80 us
    if (vma311_wait(81, 1) == VMA311_TIMEOUT_ERROR) //if it stays on 1 for longer
    {
        return VMA311_TIMEOUT_ERROR;
    }
    return VMA311_OK;
}

vma311_status_t vma311_read_byte(uint8_t *byte) // this function reads one byte 
{
    for (int i = 0; i < 8; ++i)
    {
        if (vma311_wait(50, 0) == VMA311_TIMEOUT_ERROR) //before transmitting a bit, the sensor normally begins with a 50 us low level on the pin
        {
            return VMA311_TIMEOUT_ERROR;
        }
        if (vma311_wait(70, 1) > 28)
        {
            *byte |= (1 << (7 - i)); //the sensor sends the highest bit first
        }                            
    }
    return VMA311_OK;
}

vma311_status_t vma311_check_crc(uint8_t data[])
{
    int sum = 0;
    for (int i = 0; i < 4; ++i)//here we compute the sum of each value sent by the sensor
    {
        sum += data[i];
    }
    if (sum != data[4])//if the sum is not equal to the checksum there's a communication error
    {
        return VMA311_CRC_ERROR;
    }
    else//otherwise the data is fine
    {
        return VMA311_OK;
    }
}
