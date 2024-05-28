#include "main.h"

#define ROTL8(x, shift) ((uint8_t)((x) << (shift)) | ((x) >> (8 - (shift))))

uint8_t sbox[256];
uint8_t hexArray[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t state_text[16];
uint8_t key[16] = "uwkeucarscobvydv";
uint8_t state_matrix[4][4];
uint8_t newKeyGround[16];
uint8_t expandedKey[176];

UART_HandleTypeDef huart1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
void initialize_aes_sbox(uint8_t sbox[256]);
void stringToHexArray(uint8_t *input, uint8_t *output, int inputLength);
uint8_t Rcon(uint8_t i);
void generateRandomKey(unsigned char *key, int keySize);
uint8_t Sbox(uint8_t byte);
void KeyExpansion(uint8_t *key, uint8_t *expandedKey);
uint8_t gmul(uint8_t a, uint8_t b);
void mixColumns(uint8_t state[4][4]);
void convertTo2D(uint8_t arr[16], int n, uint8_t matrix[4][4]);
void convertTo1D(uint8_t matrix[4][4], int n, uint8_t arr[16]);
void addGroundKey(uint8_t arr1[], uint8_t arr2[], uint8_t result[], int length);
void shiftRow(uint8_t matrix[4][4]);
void aesAlgorithm(uint8_t dataSend[16]);

uint8_t data_transmit[16] = "Hello Ph";
uint8_t data_receive[16];

int main(void)
{

  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_USART1_UART_Init();

  while (1)
  {
    /* USER CODE END WHILE */
    if (HAL_UART_Receive(&huart1, data_receive, 16, 1000) == HAL_OK)
    {
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
      for (int i = 0; i < 16; i++)
      {
        hexArray[i] = data_receive[i];
      }

      aesAlgorithm(data_transmit);
      HAL_UART_Transmit(&huart1, data_transmit, 16, 1000);
      HAL_Delay(1000);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
    }
    else
    {
      __NOP();
    }
    // HAL_UART_Transmit(&huart1, data_transmit, 8, 1000);
    // HAL_Delay(5000);
    /* USER CODE BEGIN 3 */
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void initialize_aes_sbox(uint8_t sbox[256])
{
  uint8_t p = 1, q = 1;

  /* loop invariant: p * q == 1 in the Galois field */
  do
  {
    /* multiply p by 3 */
    p = p ^ (p << 1) ^ (p & 0x80 ? 0x1B : 0);

    // printf("%d\n", p);
    /* divide q by 3 (equals multiplication by 0xf6) */
    q ^= q << 1;
    q ^= q << 2;
    q ^= q << 4;
    q ^= q & 0x80 ? 0x09 : 0;

    /* compute the affine transformation */
    uint8_t xformed = q ^ ROTL8(q, 1) ^ ROTL8(q, 2) ^ ROTL8(q, 3) ^ ROTL8(q, 4);

    sbox[p] = xformed ^ 0x63;
  } while (p != 1);

  /* 0 is a special case since it has no inverse */
  sbox[0] = 0x63;
}

void stringToHexArray(uint8_t *input, uint8_t *output, int inputLength)
{
  for (int i = 0; i < inputLength; i++)
  {
    output[i] = input[i];
  }
}

// Hàm để thực hiện phép toán Rcon (lũy thừa của 2 trong trường GF(2^8))
uint8_t Rcon(uint8_t i)
{
  uint8_t rcon[] = {
      0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36,
      // Các giá trị khác cho các vòng sau (nếu cần)
  };
  return rcon[i - 1]; // Trả về giá trị Rcon tương ứng
}

void generateRandomKey(unsigned char *key, int keySize)
{
  srand((unsigned int)time(NULL)); // Khởi tạo bộ sinh số ngẫu nhiên
  for (int i = 0; i < keySize; i++)
  {
    key[i] = (unsigned char)(rand() % 256); // Tạo một byte ngẫu nhiên
  }
}

// Hàm thực hiện phép thay thế S-box
uint8_t Sbox(uint8_t byte)
{

  return sbox[byte];
}

// Hàm tạo khóa mở rộng từ khóa gốc
void KeyExpansion(uint8_t *key, uint8_t *expandedKey)
{
  uint8_t temp[4];

  // Sao chép khóa gốc vào khóa mở rộng
  for (int i = 0; i < 16; i++)
  {
    expandedKey[i] = key[i];
  }

  // Bắt đầu tạo khóa mở rộng
  for (int i = 16; i < 176; i += 4)
  {
    // Lấy 4 byte cuối cùng từ khóa mở rộng trước đó
    for (int j = 0; j < 4; j++)
    {
      temp[j] = expandedKey[i - 4 + j];
    }

    // Mỗi 16 byte, thực hiện phép toán trên temp
    if (i % 16 == 0)
    {
      // Phép toán RotWord
      uint8_t t = temp[0];
      temp[0] = temp[1];
      temp[1] = temp[2];
      temp[2] = temp[3];
      temp[3] = t;

      // Phép toán SubWord (S-box)
      for (int j = 0; j < 4; j++)
      {
        temp[j] = sbox[temp[j]];
      }

      // Phép toán Rcon
      temp[0] = temp[0] ^ Rcon(i / 16);
    }

    // XOR temp với 4 byte trước đó của khóa mở rộng để tạo ra 4 byte mới
    for (int j = 0; j < 4; j++)
    {
      expandedKey[i + j] = expandedKey[i - 16 + j] ^ temp[j];
    }
  }
}

uint8_t gmul(uint8_t a, uint8_t b)
{
  uint8_t p = 0;
  uint8_t hi_bit_set;
  for (int counter = 0; counter < 8; counter++)
  {
    if ((b & 1) == 1)
      p ^= a;
    hi_bit_set = (a & 0x80);
    a <<= 1;
    if (hi_bit_set == 0x80)
      a ^= 0x1b;
    b >>= 1;
  }
  return p;
}

// Hàm thực hiện bước MixColumns
void mixColumns(uint8_t state[4][4])
{
  uint8_t tmp[4][4];
  int i, j;
  for (i = 0; i < 4; ++i)
  {
    for (j = 0; j < 4; ++j)
    {
      tmp[j][i] = gmul(state[j][i], 2) ^
                  gmul(state[(j + 1) % 4][i], 3) ^
                  gmul(state[(j + 2) % 4][i], 1) ^
                  gmul(state[(j + 3) % 4][i], 1);
    }
  }
  for (i = 0; i < 4; ++i)
  {
    for (j = 0; j < 4; ++j)
    {
      state[j][i] = tmp[j][i];
    }
  }
}

void convertTo2D(uint8_t arr[16], int n, uint8_t matrix[4][4])
{
  int size = sqrt(n);
  for (int i = 0; i < size; ++i)
  {
    for (int j = 0; j < size; ++j)
    {
      matrix[i][j] = arr[j * size + i];
    }
  }
}

void convertTo1D(uint8_t matrix[4][4], int n, uint8_t arr[16])
{
  for (int i = 0; i < n; ++i)
  {
    for (int j = 0; j < n; ++j)
    {
      arr[j * n + i] = matrix[i][j];
    }
  }
}

void addGroundKey(uint8_t arr1[], uint8_t arr2[], uint8_t result[], int length)
{
  for (int i = 0; i < length; ++i)
  {
    result[i] = arr1[i] ^ arr2[i];
  }
}

void shiftRow(uint8_t matrix[4][4])
{
  uint8_t p[4];
  for (int i = 0; i < 4; ++i)
  {
    for (int k = 0; k < 4; ++k)
    {
      p[k] = matrix[i][k];
    }
    for (int j = 0; j < 4; ++j)
    {
      matrix[i][j] = p[(j + i) % 4];
    }
  }
}

void aesAlgorithm(uint8_t dataSend[16])
{

  // Chuyển đổi và lưu vào hexArray
  // Thêm padding 0x00 để đủ 16 byte
  // for(int i = 8; i < 16; i++) {
  //     hexArray[i] = 0x00;
  // }
  initialize_aes_sbox(sbox);

  // generateRandomKey(key, sizeof(key)); // Tạo khóa ngẫu nhiên

  // printf("Khóa 16 byte ngẫu nhiên: ");
  // for(int i = 0; i < sizeof(key); i++) {
  //     printf("%02x", key[i]); // In khóa dưới dạng hex
  //     if(i < sizeof(key) - 1) printf(":");
  // }
  // printf("\n");
  // printf("Khóa 16 byte ngẫu nhiên: ");
  // for(int i = 0; i < sizeof(key); i++) {
  //     printf("%c", key[i]); // In khóa dưới dạng hex
  // }

  KeyExpansion(key, expandedKey);

  // // In khóa mở rộng ra màn hình (chỉ mục đích minh họa)
  // for (int i = 0; i < 176; i++) {
  //     if (i % 16 == 0) printf("\n");
  //     printf("%02X ", expandedKey[i]);
  // }

  addGroundKey(hexArray, key, state_text, 16);

  for (int i = 0; i < 9; ++i)
  {
    for (int j = 0; j < 16; ++j)
    {
      state_text[j] = sbox[state_text[j]];
      newKeyGround[j] = expandedKey[16 * (i + 1) + j];
    }
    convertTo2D(state_text, 16, state_matrix);
    shiftRow(state_matrix);
    mixColumns(state_matrix);
    convertTo1D(state_matrix, 4, state_text);
    addGroundKey(state_text, newKeyGround, state_text, 16);
    // convertTo2D(state_text, 16, state_matrix);
  }

  for (int j = 0; j < 16; ++j)
  {
      state_text[j] = sbox[state_text[j]];
  }
  convertTo2D(state_text, 16, state_matrix);
  shiftRow(state_matrix);
  convertTo1D(state_matrix, 4, state_text);
  for (int j = 0; j < 16; ++j)
  {
    newKeyGround[j] = expandedKey[16 * 10 + j];
  }
  addGroundKey(state_text, newKeyGround, state_text, 16);

  // printf("\n");
  // printf("\n");
  // printf("\n");

  for (int i = 0; i < 16; ++i)
  {
    dataSend[i] = state_text[i];
  }

  // printf("\n");

  // for (int i = 0; i < 16; ++i)
  // {
  //     printf("%c ", state_text[i]);
  // }
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
