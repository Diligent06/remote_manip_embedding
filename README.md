# STM32F103 Servo Control Bridge / STM32F103舵机控制桥接器

This project implements a UART-to-UART bridge for servo control using STM32F103, converting incoming commands to servo protocol.

本项目使用STM32F103实现UART到UART的舵机控制桥接，将输入命令转换为舵机协议。

## CAN Command Protocol / CAN指令协议

The system supports two main CAN commands:

系统支持两个主要的CAN指令：

### 0x06 - Write Command / 写指令

**Format:** `0x06, mode, [data...]`

**格式：** `0x06, 模式, [数据...]`

#### Modes / 模式:

- **0x00**: Disable torque / 关闭力矩
- **0x01**: Enable torque / 开启力矩  
- **0x02**: Position control / 位置控制
  - Data format: `pos1, pos2, vel1, vel2`
  - 数据格式：`位置1, 位置2, 速度1, 速度2`

### 0x07 - Read Command / 读指令

**Format:** `0x07, [any data]`

**格式：** `0x07, [任意数据]`

**Response:** Returns current position `pos1, pos2`

**响应：** 返回当前位置 `位置1, 位置2`

## Data Format / 数据格式

- **Byte Order:** Little-endian (needs verification after connection)
- **字节序：** 小端序（连接后需要验证）

## Hardware Configuration / 硬件配置

- **UART1:** Command input (115200 baud, 8N1)
- **UART2:** Servo communication (115200 baud, 8N1)
- **Target:** STM32F103C8T6

- **UART1：** 指令输入（115200波特率，8N1）
- **UART2：** 舵机通信（115200波特率，8N1）
- **目标芯片：** STM32F103C8T6
