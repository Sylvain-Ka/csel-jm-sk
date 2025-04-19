/**
 * Copyright 2025 University of Applied Sciences Western Switzerland / Fribourg
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Project: HEIA-FR / HES-SO MSE - MA-CSEL1 Laboratory
 *
 * Purpose: Board specific definitions
 *
 * Autĥors: Julien Michel / Sylvain Kämpfer
 * Date:    18.04.2025
 */

#ifndef BOARD_H
#define BOARD_H

/*
* GPIO paths
*/
#define GPIO_EXPORT "/sys/class/gpio/export"
#define GPIO_UNEXPORT "/sys/class/gpio/unexport"

/*
* status led - gpioa.10 --> gpio10
* power led  - gpiol.10 --> gpio362
*/
#define GPIO_LED "/sys/class/gpio/gpio10"
#define LED "10"

/*
* button k1 - gpioa.0 --> gpio0
* button k2 - gpioa.2 --> gpio2
* button k3 - gpioa.3 --> gpio3
*/
#define GPIO_BUTTON_K1 "/sys/class/gpio/gpio0"
#define GPIO_BUTTON_K2 "/sys/class/gpio/gpio2"
#define GPIO_BUTTON_K3 "/sys/class/gpio/gpio3"
#define BUTTON_K1 "0"
#define BUTTON_K2 "2"
#define BUTTON_K3 "3"

#endif // BOARD_H