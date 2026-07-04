/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

#include <drivers/blink/blink.h>

#include <app_version.h>

LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

#define BLINK_PERIOD_MS_STEP 100U
#define BLINK_PERIOD_MS_MAX 1000U
#define SENSOR_THREAD_STACK_SIZE 1024
#define LED_THREAD_STACK_SIZE 512
#define SENSOR_THREAD_PRIORITY 5
#define LED_THREAD_PRIORITY 5

/* Message queue for communication between threads */
K_MSGQ_DEFINE(period_msgq, sizeof(unsigned int), 10, 4);

/* Global device pointers */
static const struct device *sensor_dev;
static const struct device *blink_dev;

/* Sensor thread - reads sensor values and sends period updates */
void sensor_thread(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);

    int ret;
    struct sensor_value last_val = {0}, val;
    unsigned int period_ms = 0;

    LOG_INF("Sensor thread started");

    while (1)
    {
        ret = sensor_sample_fetch(sensor_dev);
        if (ret < 0)
        {
            LOG_ERR("Could not fetch sample (%d)", ret);
            k_sleep(K_MSEC(100));
            continue;
        }

        ret = sensor_channel_get(sensor_dev, SENSOR_CHAN_PROX, &val);
        if (ret < 0)
        {
            LOG_ERR("Could not get sample (%d)", ret);
            k_sleep(K_MSEC(100));
            continue;
        }

        /* Detect rising edge (0 -> 1 transition) */
        if ((last_val.val1 == 0) && (val.val1 == 1))
        {
            if (period_ms == 0U)
            {
                period_ms = BLINK_PERIOD_MS_MAX;
            }
            else
            {
                period_ms -= BLINK_PERIOD_MS_STEP;
            }

            LOG_INF("Proximity detected, new period: %u ms", period_ms);

            /* Send new period to LED thread */
            ret = k_msgq_put(&period_msgq, &period_ms, K_NO_WAIT);
            if (ret < 0)
            {
                LOG_WRN("Message queue full, dropping period update");
            }
        }

        last_val = val;

        k_sleep(K_MSEC(100));
    }
}

/* LED thread - receives period updates and controls LED */
void led_thread(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);

    int ret;
    unsigned int period_ms;

    LOG_INF("LED thread started");

    /* Turn off LED initially */
    ret = blink_off(blink_dev);
    if (ret < 0)
    {
        LOG_ERR("Could not turn off LED (%d)", ret);
        return;
    }

    while (1)
    {
        /* Wait for period update from sensor thread */
        ret = k_msgq_get(&period_msgq, &period_ms, K_FOREVER);
        if (ret == 0)
        {
            ret = blink_set_period_ms(blink_dev, period_ms);
            if (ret < 0)
            {
                LOG_ERR("Could not set LED period (%d)", ret);
            }
            else
            {
                LOG_INF("LED period set to %u ms", period_ms);
            }
        }
    }
}

/* Thread definitions */
K_THREAD_DEFINE(sensor_tid, SENSOR_THREAD_STACK_SIZE,
                sensor_thread, NULL, NULL, NULL,
                SENSOR_THREAD_PRIORITY, 0, 0);

K_THREAD_DEFINE(led_tid, LED_THREAD_STACK_SIZE,
                led_thread, NULL, NULL, NULL,
                LED_THREAD_PRIORITY, 0, 0);

int main(void)
{
    printk("Zephyr Example Application %s\n", APP_VERSION_STRING);

    sensor_dev = DEVICE_DT_GET(DT_NODELABEL(example_sensor));
    if (!device_is_ready(sensor_dev))
    {
        LOG_ERR("Sensor not ready");
        return 0;
    }

    blink_dev = DEVICE_DT_GET(DT_NODELABEL(blink_led));
    if (!device_is_ready(blink_dev))
    {
        LOG_ERR("Blink LED not ready");
        return 0;
    }

    printk("Use the sensor to change LED blinking period\n");
    LOG_INF("Application initialized, threads running");

    /* Main thread can sleep or perform other tasks */
    while (1)
    {
        LOG_INF("Heartbeat...");
        k_sleep(K_MSEC(5000));
    }

    return 0;
}