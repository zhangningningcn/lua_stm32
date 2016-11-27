/**
  @page USBD_VCP  USB Device VCP example
  
  @verbatim
  ******************** (C) COPYRIGHT 2015 STMicroelectronics *******************
  * @file    readme.txt 
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    09-November-2015
  * @brief   Description of the USB Device VCP (Virtual Com Port) example
  ******************************************************************************
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License
  *   
  ******************************************************************************
   @endverbatim

   
@par Example Description 

This Demo presents the implementation of a Virtual Com Port (VCP) capability in 
the STM32F207xx, STM32F407xx, STM32F429xx and STM32F107xx devices. 

It illustrates an implementation of the CDC class following the PSTN subprotocol.
The VCP example allows the STM32 device to behave as a USB-to-RS232 bridge.
 - On one side, the STM32 communicates with host (PC) through USB interface in Device mode.
 - On the other side, the STM32 communicates with other devices (same host, other host,
   other devices…) through the USART interface (RS232).

The support of the VCP interface is managed through the ST Virtual Com Port driver
available for download from www.st.com.
This example can be customized to communicate with interfaces other than USART.
The VCP example works in High and Full speed modes.

When the VCP application starts, the USB device is enumerated as serial communication
port and can be configured in the same way (baudrate, data format, parity, stop bit
length…).
To test this example, you can use one of the following configurations:
 - Configuration 1: Connect USB cable to host and USART (RS232) to a different host
   (PC or other device) or to the same host. In this case, you can open two hyperterminal-like
   terminals to send/receive data to/from host to/from device.
 - Configuration 2: Connect USB cable to Host and connect USART TX pin to USART
   RX pin on the evaluation board (Loopback mode). In this case, you can open one
   terminal (relative to USB com port or USART com port) and all data sent from this
   terminal will be received by the same terminal in loopback mode. This mode is useful
   for test and performance measurements.

To use a different CDC communication interface you can use the template CDC 
interface provided in folder \Libraries\STM32_USB_Device_Library\Class\cdc.

@note When transferring a big file (USB OUT transfer) user have to adapt the size
      of IN buffer, for more details refer to usbd_conf.h file (APP_RX_DATA_SIZE constant).


This example works
  - in high speed (HS) when the STM322xG-EVAL, the STM324xG-EVAL or the STM324x9I-EVAL  
    board and the USB OTG HS peripheral are used.
  - in full speed (FS) when the STM322xG-EVAL, the STM324xG-EVAL or the STM324x9I-EVAL board and the
    USB OTG FS peripheral are used, or when using the STM3210C-EVAL board.
  - in HS-IN-FS when the STM324x9I-EVAL board and the USB OTG HS-IN-FS peripheral are used.
    

@par Hardware and Software environment 

   - This example has been tested with STM3210C-EVAL RevB (STM32F107xx devices), 
     STM322xG-EVAL RevB (STM32F207xx), STM324xG-EVAL RevB (STM32F407xx) and STM324x9I-EVAL RevB (STM32F429xx)   

  - STM3210C-EVAL Setup 
    - Use CN2 connector to connect the board to a PC host
    - Use CN6 connector to connect the board to PC (or to another evaluation board) over 
      RS232 (USART) serial cable.
    - Jumper JP16 should be connected in position 2-3.
    - For loopback mode test: remove RS232 cable on CN6 and connect directly USART
      TX and RX pins: PD5 and PD6 (with a cable or a jumper)
       
  - STM322xG-EVAL Setup
    - Use CN8 connector to connect the board to a PC host when using USB OTG FS peripheral
    - Use CN9 connector to connect the board to a PC host when using USB OTG HS peripheral
    - Use CN16 connector to connect the board to PC (or to another evaluation board) over 
      RS232 (USART) serial cable.
		@note Make sure that :
		 - Please ensure that jumper JP31 is fitted when using USB OTG HS 
		 - ensure that jumper JP22 is connected in position 1-2.
    - For loopback mode test: remove RS232 cable on CN16 and connect directly USART
      TX and RX pins: PC10 and PC11 (with a cable or a jumper)

  - STM324xG-EVAL Setup
    - Use CN8 connector to connect the board to a PC host when using USB OTG FS peripheral
    - Use CN9 connector to connect the board to a PC host when using USB OTG HS peripheral
    - Use CN16 connector to connect the board to PC (or to another evaluation board) over 
      RS232 (USART) serial cable.
    @note Make sure that :
		 - Please ensure that jumper JP31 is fitted when using USB OTG HS 
		 - ensure that jumper JP22 is connected in position 1-2.
    - For loopback mode test: remove RS232 cable on CN16 and connect directly USART
      TX and RX pins: PC10 and PC11 (with a cable or a jumper)  

  - STM324x9I-EVAL Set-up
    - Use CN14 connector to connect the board to host when using USB OTG FS peripheral
    - Use CN9  connector to connect the board to host when using USB OTG HS peripheral
    - Use CN15  connector to connect the board to host when using USB OTG HS-IN-FS peripheral
      Note that some FS signals are shared with the HS ULPI bus, so some PCB rework is needed.
              For more details, refer to section "USB OTG2 HS & FS" in STM324x9I-EVAL Evaluation Board 
              User Manual.
        @note Make sure that :
        - Please ensure that jumper JP16 is not fitted when using USB OTG FS
        - Use CN8 connector to connect the board to PC (or to another evaluation board) over 
          RS232 (USART) serial cable.
        - For loopback mode test: remove RS232 cable on CN16 and connect directly USART
          TX and RX pins: PC10 and PC11 (with a cable or a jumper) 	  
  
  
@par How to use it ?

In order to make the program work, you must do the following:
  - Open your preferred toolchain
  - In the workspace toolbar select the project config:
    - STM322xG-EVAL_USBD-HS: to configure the project for STM32F207xx devices and use USB OTG HS peripheral
    - STM322xG-EVAL_USBD-FS: to configure the project for STM32F207xx devices and use USB OTG FS peripheral

    - STM324xG-EVAL_USBD-HS: to configure the project for STM32F407xx devices and use USB OTG HS peripheral
    - STM324xG-EVAL_USBD-FS: to configure the project for STM32F407xx devices and use USB OTG FS peripheral

    - STM324x9i-EVAL_USBD-HS:to configure the project for STM32F429xx devices and use USB OTG HS peripheral
    - STM324x9i-EVAL_USBD-FS:to configure the project for STM32F429xx devices and use USB OTG FS peripheral
    - STM324x9i-EVAL_USBD-HS-IN-FS: to configure the project for STM32F429xx devices and use USB OTG HS-IN-FS peripheral

    - STM3210C-EVAL_USBD-FS: to configure the project for STM32F107xx devices  
  
  - Rebuild all files and load your image into target memory
  - Run the application

@note Known Limitations
      This example retarget the C library printf() function to the EVAL board’s LCD
      screen (C library I/O redirected to LCD) to display some Library and user debug
      messages. TrueSTUDIO Lite version does not support I/O redirection, and instead
      have do-nothing stubs compiled into the C runtime library. 
      As consequence, when using this toolchain no debug messages will be displayed
      on the LCD (only some control messages in green will be displayed in bottom of
      the LCD). To use printf() with TrueSTUDIO Professional version, just include the
      TrueSTUDIO Minimal System calls file "syscalls.c" provided within the toolchain.
      It contains additional code to support printf() redirection.
    
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
