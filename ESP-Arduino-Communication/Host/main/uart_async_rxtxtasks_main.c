#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define RX_BUF_SIZE 1024

#define TXD_PIN GPIO_NUM_17 // Change to the appropriate TX pin for UART2
#define RXD_PIN GPIO_NUM_16 // Change to the appropriate RX pin for UART2

// Define the expected hex array for "/?!\r\n"
const uint8_t EXPECTED_HEX[] = {0x2F, 0x3F, 0x21, 0x0D, 0x0A};
#define EXPECTED_HEX_LEN (sizeof(EXPECTED_HEX) / sizeof(EXPECTED_HEX[0]))
const uint8_t IDENTIFICATION_MESSAGE[] = {0x2F, 0x4C, 0x47, 0x5A, 0x34, 0x5A, 0x4D, 0x46, 0x31, 0x30, 0x30, 0x41, 0x43, 0x2E, 0x4D, 0x32, 0x39, 0x0D, 0x0A};
#define IDENTIFICATION_MESSAGE_LEN (sizeof(IDENTIFICATION_MESSAGE) / sizeof(IDENTIFICATION_MESSAGE[0]))
const uint8_t REQUEST_DATA[] = {0x06, '0', '4', '0', '\r', '\n'}; // Command in hexadecimal representation
#define REQUEST_DATA_LEN (sizeof(REQUEST_DATA) / sizeof(REQUEST_DATA[0]))

unsigned char hexData[] = {
    0x02, // STX (Start of Text)
    0x46, 0x2E, 0x46, 0x28, 0x30, 0x30, 0x29, 0x0D, 0x0A, // F.F(00)CRLF
    0x31, 0x2E, 0x38, 0x2E, 0x30, 0x28, 0x30, 0x30, 0x30, 0x30, 0x35, 0x32, 0x2E, 0x33, 0x33, 0x37, 0x2A, 0x6B, 0x57, 0x68, 0x29, 0x0D, 0x0A,
    0x32, 0x2E, 0x38, 0x2E, 0x30, 0x28, 0x30, 0x30, 0x33, 0x37, 0x36, 0x2E, 0x34, 0x33, 0x32, 0x2A, 0x6B, 0x57, 0x68, 0x29, 0x0D, 0x0A,
    0x33, 0x2E, 0x38, 0x2E, 0x30, 0x28, 0x30, 0x30, 0x31, 0x34, 0x35, 0x2E, 0x38, 0x37, 0x35, 0x2A, 0x6B, 0x76, 0x61, 0x72, 0x68, 0x29, 0x0D, 0x0A,
    0x34, 0x2E, 0x38, 0x2E, 0x30, 0x28, 0x30, 0x30, 0x30, 0x31, 0x30, 0x2E, 0x37, 0x32, 0x34, 0x2A, 0x6B, 0x76, 0x61, 0x72, 0x68, 0x29, 0x0D, 0x0A,
    0x31, 0x35, 0x2E, 0x38, 0x2E, 0x30, 0x28, 0x30, 0x30, 0x30, 0x34, 0x32, 0x38, 0x2E, 0x37, 0x36, 0x39, 0x2A, 0x6B, 0x57, 0x68, 0x29, 0x0D, 0x0A,
    0x33, 0x32, 0x2E, 0x37, 0x28, 0x32, 0x33, 0x31, 0x2A, 0x56, 0x29, 0x0D, 0x0A,
    0x35, 0x32, 0x2E, 0x37, 0x28, 0x32, 0x33, 0x30, 0x2A, 0x56, 0x29, 0x0D, 0x0A,
    0x37, 0x32, 0x2E, 0x37, 0x28, 0x32, 0x33, 0x30, 0x2A, 0x56, 0x29, 0x0D, 0x0A,
    0x33, 0x31, 0x2E, 0x37, 0x28, 0x30, 0x30, 0x2E, 0x30, 0x30, 0x30, 0x2A, 0x41, 0x29, 0x0D, 0x0A,
    0x35, 0x31, 0x2E, 0x37, 0x28, 0x30, 0x30, 0x2E, 0x30, 0x30, 0x30, 0x2A, 0x41, 0x29, 0x0D, 0x0A,
    0x37, 0x31, 0x2E, 0x37, 0x28, 0x30, 0x30, 0x2E, 0x30, 0x30, 0x30, 0x2A, 0x41, 0x29, 0x0D, 0x0A,
    0x31, 0x33, 0x2E, 0x37, 0x28, 0x2D, 0x2E, 0x2D, 0x2D, 0x29, 0x0D, 0x0A,
    0x31, 0x34, 0x2E, 0x37, 0x28, 0x35, 0x30, 0x2E, 0x30, 0x2A, 0x48, 0x7A, 0x29, 0x0D, 0x0A,
    0x43, 0x2E, 0x31, 0x2E, 0x30, 0x28, 0x34, 0x30, 0x37, 0x39, 0x39, 0x33, 0x39, 0x30, 0x29, 0x0D, 0x0A,
    0x30, 0x2E, 0x30, 0x28, 0x34, 0x30, 0x37, 0x39, 0x39, 0x33, 0x39, 0x30, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x29, 0x0D, 0x0A,
    0x43, 0x2E, 0x31, 0x2E, 0x31, 0x28, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x29, 0x0D, 0x0A,
    0x30, 0x2E, 0x32, 0x2E, 0x30, 0x28, 0x4D, 0x32, 0x39, 0x29, 0x0D, 0x0A,
    0x31, 0x36, 0x2E, 0x37, 0x28, 0x30, 0x30, 0x30, 0x2E, 0x30, 0x30, 0x2A, 0x6B, 0x57, 0x29, 0x0D, 0x0A,
    0x31, 0x33, 0x31, 0x2E, 0x37, 0x28, 0x30, 0x30, 0x30, 0x2E, 0x30, 0x30, 0x2A, 0x6B, 0x56, 0x41, 0x72, 0x29, 0x0D, 0x0A,
    0x43, 0x2E, 0x35, 0x2E, 0x30, 0x28, 0x36, 0x34, 0x30, 0x32, 0x29, 0x0D, 0x0A,
    0x43, 0x2E, 0x37, 0x2E, 0x30, 0x28, 0x30, 0x30, 0x36, 0x34, 0x29, 0x0D, 0x0A,
    0x21, 0x0D, 0x0A, // !CRLF
    0x03, // ETX (End of Text)
    0x08  // BS (Backspace)
};

void appendRandomValues() {
    // Seed the random number generator
    srand(time(NULL));
    // Generating random values for voltage and current nodes
    int voltage1 = rand() % 400; // Random voltage value up to 400V
    int voltage2 = rand() % 400; // Random voltage value up to 400V
    int voltage3 = rand() % 400; // Random voltage value up to 400V
    double current1 = (rand() % 10000) / 1000.0; // Random current value up to 10A with 3 decimal places
    double current2 = (rand() % 10000) / 1000.0; // Random current value up to 10A with 3 decimal places
    double current3 = (rand() % 10000) / 1000.0; // Random current value up to 10A with 3 decimal places

    // Appending the random voltage and current values to the hexData array
    // Update voltage1 value at positions 36, 37, 39, 40, 41, 42, 43, 44, 45
    hexData[0x24] = (voltage1 / 100) + '0';
    hexData[0x25] = ((voltage1 % 100) / 10) + '0';
    hexData[0x27] = (voltage1 % 10) + '0';
    hexData[0x28] = '.';
    hexData[0x29] = 'V';
    hexData[0x2A] = ')';
    hexData[0x2B] = 'C';
    hexData[0x2C] = 'R';
    hexData[0x2D] = 'L';
    // Update voltage2 value at positions 46, 47, 49, 50, 51, 52, 53, 54, 55
    hexData[0x2E] = (voltage2 / 100) + '0';
    hexData[0x2F] = ((voltage2 % 100) / 10) + '0';
    hexData[0x31] = (voltage2 % 10) + '0';
    hexData[0x32] = '.';
    hexData[0x33] = 'V';
    hexData[0x34] = ')';
    hexData[0x35] = 'C';
    hexData[0x36] = 'R';
    hexData[0x37] = 'L';
    // Update voltage3 value at positions 56, 57, 59, 60, 61, 62, 63, 64, 65
    hexData[0x38] = (voltage3 / 100) + '0';
    hexData[0x39] = ((voltage3 % 100) / 10) + '0';
    hexData[0x3B] = (voltage3 % 10) + '0';
    hexData[0x3C] = '.';
    hexData[0x3D] = 'V';
    hexData[0x3E] = ')';
    hexData[0x3F] = 'C';
    hexData[0x40] = 'R';
    hexData[0x41] = 'L';
    // Update current1 value at positions 70, 71, 72, 73, 74, 75, 76, 77
    hexData[0x46] = ((int)current1 / 10) + '0';
    hexData[0x47] = ((int)current1 % 10) + '0';
    hexData[0x48] = '.';
    hexData[0x49] = ((int)(current1 * 10) % 10) + '0';
    hexData[0x4A] = ((int)(current1 * 100) % 10) + '0';
    hexData[0x4B] = ((int)(current1 * 1000) % 10) + '0';
    hexData[0x4C] = 'A';
    hexData[0x4D] = ')';
    hexData[0x4E] = 'C';
    hexData[0x4F] = 'R';
    hexData[0x50] = 'L';
    // Update current2 value at positions 78, 79, 80, 81, 82, 83, 84, 85
    hexData[0x51] = ((int)(current2 / 10)) + '0';
    hexData[0x52] = ((int)current2 % 10) + '0';
    hexData[0x53] = '.';
    hexData[0x54] = ((int)(current2 * 10) % 10) + '0';
    hexData[0x55] = ((int)(current2 * 100) % 10) + '0';
    hexData[0x56] = ((int)(current2 * 1000) % 10) + '0';
    hexData[0x57] = 'A';
    hexData[0x58] = ')';
    hexData[0x59] = 'C';
    hexData[0x5A] = 'R';
    hexData[0x5B] = 'L';
    // Update current3 value at positions 86, 87, 88, 89, 90, 91, 92, 93
    hexData[0x5C] = ((int)current3 / 10) + '0';
    hexData[0x5D] = ((int)current3 % 10) + '0';
    hexData[0x5E] = '.';
    hexData[0x5F] = ((int)(current3 * 10) % 10) + '0';
    hexData[0x60] = ((int)(current3 * 100) % 10) + '0';
    hexData[0x61] = ((int)(current3 * 1000) % 10) + '0';
    hexData[0x62] = 'A';
    hexData[0x63] = ')';
    hexData[0x64] = 'C';
    hexData[0x65] = 'R';
    hexData[0x66] = 'L';
}

void extractAndPrintDataLines() {
    int i = 1; // Start from the second byte since the first byte is STX
    while (hexData[i] != 0x03) { // Continue until reaching ETX (End of Text)
        if (hexData[i] == 0x0D && hexData[i + 1] == 0x0A) { // Check for CRLF (Carriage Return + Line Feed)
            i += 2; // Move to the start of the next line
        } else {
            // Print ASCII text for the current line until the next CRLF or ETX
            while (hexData[i] != 0x0D && hexData[i] != 0x03) {
                printf("%c", hexData[i]);
                i++;
            }
            printf("\n"); // Print new line after each data line
        }
    }
}

void init_uart(void) {
    const uart_config_t uart_config = {
        .baud_rate = 300,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    // Install and configure UART2
    uart_driver_install(UART_NUM_2, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_2, &uart_config);
    uart_set_pin(UART_NUM_2, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

bool compareArrays(const uint8_t* arr1, const uint8_t* arr2, int len) {
    for (int i = 0; i < len; i++) {
        if (arr1[i] != arr2[i]) {
            return false; // Arrays are different
        }
    }
    return true; // Arrays are identical
}

void sendIdentificationMessage(void) {
    int bytes_written = uart_write_bytes(UART_NUM_2, (const char*)IDENTIFICATION_MESSAGE, IDENTIFICATION_MESSAGE_LEN);
    if (bytes_written < 0) {
        ESP_LOGE("UART", "Failed to write data to UART");
    } else {
        ESP_LOGI("UART", "Sent Identification Message");
    }
}

void sendResponseData(void) {
    int bytes_written = uart_write_bytes(UART_NUM_2, (const char*)hexData, sizeof(hexData));
    if (bytes_written < 0) {
        ESP_LOGE("UART", "Failed to write data to UART");
    } else {
        ESP_LOGI("UART", "Sent Response Data");
    }
}

static void rx_task(void *arg) {
    static const char *RX_TASK_TAG = "RX_TASK";
    uart_flush_input(UART_NUM_2);
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE);
    memset(data, 0, RX_BUF_SIZE);
    while (1) {
        const int rxBytes = uart_read_bytes(UART_NUM_2, data, RX_BUF_SIZE, 1000 / portTICK_PERIOD_MS);
        if (rxBytes > 0) {
            ESP_LOGI(RX_TASK_TAG, "Read %d bytes:", rxBytes);
            for (int i = 0; i < rxBytes; i++) {
                ESP_LOGI(RX_TASK_TAG, "Byte %d: 0x%02X", i, data[i]);
            }
            if (rxBytes == EXPECTED_HEX_LEN && compareArrays(data, EXPECTED_HEX, EXPECTED_HEX_LEN)) {
                sendIdentificationMessage(); // Call function on match success
            }
            else if(rxBytes == REQUEST_DATA_LEN && compareArrays(data, REQUEST_DATA, REQUEST_DATA_LEN)) {
                sendResponseData();
            }
        }
    }
    free(data);
}

void app_main(void) {
    init_uart();
    extractAndPrintDataLines();
    xTaskCreate(rx_task, "uart_rx_task", 1024*2, NULL, configMAX_PRIORITIES, NULL);
}
