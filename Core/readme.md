## 修改记录


### 20250715
增添关门信号==1和平层信号==1时，每隔1秒检测门前有无物体，检测到就持续输出，直到下一个检测中没有检测到物体

添加学习失败：学习超过3次，赋予失败，停止尝试

距离学习，或关门时间学习，一个指令执行一次，执行成功或失败都结束

模式：设置，取消/结束
学习：设置/启动，保存（暂时使用取消作为写入）

修改结束通信协议

将学习成功 与 传感器是否进行学习状态分离



### 20250716 开关门信号，IO使能，关门时间
```C
uint32_t closeDoorCmd;      关门指令
uint32_t openDoorCmd;       开门指令
uint32_t OLS;               开门到位
uint32_t CLS;               关门到位
uint32_t LevelingSignal;    平层

typedef struct{
	uint32_t closeDoorCmd;    
	uint32_t openDoorCmd;
	uint32_t OLS;
	uint32_t CLS;
	uint32_t LevelingSignal;
}ControlSystemPara;

ControlSystemPara CSpara = {0, 0, 0, 0, 0};

uint8_t ds6012outputEN = 0；// 传感器1，1启动，0关闭
uint8_t nd06outputEN = 0；// 传感器1，1启动，0关闭

```
前提条件：电梯来到当前层，CSpara.LevelingSignal = 1;
1. 初始状态：关门到位及保持关门 CSpara.LevelingSignal = 1，CSpara.CLS== 1
2. 按开门：门开始打开，CSpara.LevelingSignal = 1，CSpara.openDoorCmd == 1
3. 开门到位及保持开门：门打开到极限位置，CSpara.LevelingSignal = 1，CSpara.OLS == 1
4. 按关门：CSpara.LevelingSignal = 1，CSpara.closeDoorCmd == 1
5. 关门到位及保持关门：CSpara.LevelingSignal = 1，CSpara.CLS == 1

现在我需要在以下情况，让传感器1，2执行动作：
1. 关门到位后，每隔1s传感器1，2检查，直到关门到位取消，开始开门；
2. 开门过程，传感器1，2都关闭；
3. 开门保持时，每隔1s传感器1，2检查，
4. 开门到位取消，开始关门后，每隔10ms传感器1，2检查；直到t1后，关闭传感器1，只用传感器2检查，直到关门到位


### 代码说明：

1. **状态机设计**：
   - `STATE_CLOSED`：关门到位状态（初始状态）
   - `STATE_OPENING`：开门过程中
   - `STATE_OPENED`：开门到位状态
   - `STATE_CLOSING`：关门过程中

2. **传感器控制逻辑**：
   - **关门到位时**：每秒触发一次双传感器检查（1s周期）
   - **开门过程/到位时**：关闭所有传感器
   - **关门过程中**：
     - 前500ms(T1)：每5ms触发双传感器检查
     - 500ms后：每5ms仅触发传感器2检查

3. **定时器处理**：
   - 使用1ms定时器中断维护全局计时
   - 在关门过程中使用独立计时器(`closingPhaseTimer`)记录关门持续时间

4. **状态转换条件**：
   - 关门到位 → 开门过程：收到开门指令
   - 开门过程 → 开门到位：检测到开门到位信号
   - 开门到位 → 关门过程：收到关门指令
   - 关门过程 → 关门到位：检测到关门到位信号

### 使用注意事项：
1. 需要硬件平台提供1ms精度的定时器中断
2. 输入信号(`closeDoorCmd`, `openDoorCmd`等)需要在实际硬件中映射到真实IO
3. T1持续时间(500ms)可根据实际需求调整
4. 传感器使能信号需要连接到实际驱动电路
5. 主循环中需要实现具体的IO读写操作

这个实现满足了您需求中的所有状态转换条件和传感器控制要求，并提供了清晰的状态机结构便于维护和扩展。



### 20250717 距离学习，关门时间学习
```
Unknown_STATUS                                         -1
NotDetected_STATUS                                      0
NormalWorking_STATUS                                    1
DistanceThresholdLearning_STATUS                        2
ClosingTimeLearning_STATUS                              3
Idle_STATUS                                             4
Fault_STATUS                                            5
setUp_STATUS                                            6
```


### 20250718 关门时间学习
- 调试模式作为电梯状态
- 关门时间和距离学习都是指令，在调试模式下才能执行
- 解决关门时间计时

- 调试模式进入，修改sensor status = 2，定期向主控发信息
- 关门、距离学习，改为指令，request = 0/1，执行对应命令，暂时定一个temp status = 3
- 距离学习，在完全开门或关门时执行，获取距离信息（是否需要传感器获得当前电梯门的状态）
- 关门时间，在完全开门时执行=（需要电梯门正常运行，否则无法判断计时的开始）

Hrde52: 这是我用STM32H503开发板的程序，void ClosingTimeLearning()需要实现在currentDoorState = STATE_OPENED消失开始到nd06_data.dep[i * 4 + j] <= <= PARA_TABLE_USE.data.nd06StudyDistance - PARA_TABLE_USE.data.nd06DistanceChkThreshold的时间长度，提供思路和代码，以及解析

GitHub Copilot: 你的需求是：**测量从`currentDoorState`变为`STATE_OPENED`消失（即门开始关闭）到检测到像素被遮挡（`nd06_data.dep[i * 4 + j] <= PARA_TABLE_USE.data.nd06StudyDistance - PARA_TABLE_USE.data.nd06DistanceChkThreshold`）之间的时间长度**，用于关门时间学习。

### 实现思路

1. **等待门打开状态消失**  
   当`currentDoorState != STATE_OPENED`时，说明门开始关闭，此时记录时间起点。

2. **持续采集TOF数据**  
   在门关闭过程中，不断采集`nd06_data.dep`数据。

3. **判断像素遮挡条件**  
   当有像素满足`nd06_data.dep[i * 4 + j] <= PARA_TABLE_USE.data.nd06StudyDistance - PARA_TABLE_USE.data.nd06DistanceChkThreshold`时，记录时间终点。

4. **计算时间差**  
   用终点时间减去起点时间，得到关门时间。

5. **保存学习结果**  
   将关门时间保存到参数表或变量。

### 代码实现

建议用`HAL_GetTick()`（毫秒计时）记录时间：

````c
void ClosingTimeLearning()
{
    static uint32_t startTick = 0;
    static uint8_t learningStarted = 0;
    int32_t ret = ND06AV1C_ERROR_NONE;
    int i = 0, j = 0;

    now_ClosingTimeLearning_Flg = 1; // 正在学习

    // 1. 等待门打开状态消失，记录起始时间
    if (!learningStarted && currentDoorState != STATE_OPENED)
    {
        startTick = HAL_GetTick();
        learningStarted = 1;
        pixelOccludedFlag = 0;
    }

    // 2. 只有在学习已开始时才继续
    if (learningStarted)
    {
        ret = ND06AV1C_GetDepthAndAmpData(&g_nd06av1c_device, (uint16_t *)nd06_data.amp, (uint16_t *)nd06_data.dep);
        if (ret == ND06AV1C_GET_DATA_SUCCESS)
        {
            for (i = 1; i < 4; i++)
            {
                for (j = 1; j < 4; j++)
                {
                    if (nd06_data.dep[i * 4 + j] <= PARA_TABLE_USE.data.nd06StudyDistance - PARA_TABLE_USE.data.nd06DistanceChkThreshold)
                    {
                        pixelOccludedFlag = 1;
                        break;
                    }
                }
                if (pixelOccludedFlag)
                    break;
            }
        }

        // 3. 检测到像素遮挡，记录终止时间
        if (pixelOccludedFlag)
        {
            uint32_t closeTimeCnt = HAL_GetTick() - startTick; // 关门时间（ms）
            ClosingTimeLearningSuccessFlg = 1;
            now_ClosingTimeLearning_Flg = 0;
            times_ClosingTimeLearning = 0;
            learningStarted = 0;

            // 保存学习结果
            if (PARA_TABLE_USE.data.closingDoorTime != closeTimeCnt)
            {
                PARA_TABLE_USE.data.closingDoorTime = closeTimeCnt;
                paraTable_Write();
            }
        }
    }
}
````

### 解析

- **startTick**：记录门开始关闭的时间点（单位ms）。
- **learningStarted**：标记学习过程是否已开始，防止多次计时。
- **HAL_GetTick()**：STM32 HAL库函数，返回系统启动后的毫秒数。
- **pixelOccludedFlag**：检测到像素遮挡时置1。
- **closeTimeCnt**：关门时间，单位ms。
- **保存结果**：将学习到的关门时间写入参数表。

### 总结

这样实现后，`ClosingTimeLearning()`会自动测量门关闭到像素遮挡的时间，并保存到参数表。你可以根据实际需求调整像素判断范围和计时方式。



### 20250719 错误状态
由软件模拟持续发1，或者不发1
1）正常情况：门正在关，传感器检测到物体，IO = 1, 门状态变为正在开，直到变为完全开门
2）异常情况：门正在关，传感器检测到物体，IO = 1, 门状态变为正在关，直到完全关门；记录为一次异常，累计3次停止传感器的IO输出


Hrde52: 在这里要添加异常检测机制。1）正常情况：门正在关，传感器检测到物体，IO = 1, 门状态变为正在开，直到变为完全开门
2）异常情况：门正在关，传感器检测到物体，IO = 1, 门状态变为正在关，直到完全关门，；记录为一次异常，累计3次停止传感器的IO输出

GitHub Copilot: 你可以在 `sensor_action_control()` 函数中添加异常检测机制。建议如下：

1. 新增异常计数变量 `objectDetectionErrorCnt` 和停止IO输出标志 `stopIOOutputFlag`。
2. 在“正常情况”下不计数异常。
3. 在“异常情况”下计数，累计3次后停止IO输出（IO始终为关闭）。

代码示例：

````c
// ...existing code...

uint8_t objectDetectionErrorCnt = 0;
uint8_t stopIOOutputFlag = 0;

// 在 sensor_action_control() 函数内 ObjectDetection() 之后添加异常检测逻辑
else if (sensor_status == NormalWorking_STATUS) // 正常工作
{
    ClosingTimeLearningReqFlg = 0;
    ObjectDetection();

    // 异常检测机制
    if (ObjectIsDetectedFlag == 1)
    {
        // 正常情况：门正在关，检测到物体，门状态变为正在开
        if (currentDoorState == STATE_OPENING || currentDoorState == STATE_OPENED)
        {
            objectDetectionErrorCnt = 0; // 正常，不计数
            stopIOOutputFlag = 0;
        }
        // 异常情况：门正在关，检测到物体，门状态仍为正在关
        else if (currentDoorState == STATE_CLOSING)
        {
            objectDetectionErrorCnt++;
            if (objectDetectionErrorCnt >= 3)
            {
                stopIOOutputFlag = 1;
            }
        }
    }
    else
    {
        objectDetectionErrorCnt = 0;
        stopIOOutputFlag = 0;
    }

    // IO输出控制
    if (stopIOOutputFlag)
    {
        LEDOFF;
        HAL_GPIO_WritePin(IO_OUT_GPIO_Port, IO_OUT_Pin, GPIO_PIN_SET); // IO关闭
    }
    else
    {
        if (ObjectIsDetectedFlag == 1)
        {
            LEDON;
            HAL_GPIO_WritePin(IO_OUT_GPIO_Port, IO_OUT_Pin, GPIO_PIN_RESET); // IO输出
        }
        else
        {
            LEDOFF;
            HAL_GPIO_WritePin(IO_OUT_GPIO_Port, IO_OUT_Pin, GPIO_PIN_SET); // IO关闭
        }
    }
}
// ...existing code...
````

这样即可实现异常检测和累计机制，满足你的需求。


## 20250724持续0，错误检测
传感器在关门过程会检测有无物体，如果连续五次关门过程中都没有检测到物体，就报E003错误

## end