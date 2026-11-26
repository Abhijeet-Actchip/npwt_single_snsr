/*
 * mock_data.h
 *
 * Simulator mock sensor data feed.
 */

#ifndef __MOCK_DATA_H__
#define __MOCK_DATA_H__		1

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Starts the mock sensor data task.
 *        Feeds simulated sensor values to the GUI at a fixed interval.
 *        Uses UIPlatform_CreateTask internally.
 */
void MockDataStart(void);

#ifdef __cplusplus
}
#endif

#endif /* __MOCK_DATA_H__ */
