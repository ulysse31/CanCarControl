/*
 * Copyright (c) 2018 Particle Industries, Inc.  All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */


#ifndef __RESULT_H__
# define __RESULT_H__
#include <esp_err.h>

enum LogLevel {
    LOG_LEVEL_TRACE = ESP_LOG_DEBUG,
    LOG_LEVEL_INFO = ESP_LOG_INFO,
    LOG_LEVEL_WARN = ESP_LOG_WARN,
    LOG_LEVEL_ERROR = ESP_LOG_ERROR
};

#define CHECK(_expr) \
        ({ \
            const auto _ret = _expr; \
            if (_ret < 0) { \
                return _ret; \
            } \
            _ret; \
        })

#define CHECK_RETURN(_expr, _val) \
        ({ \
            const auto _ret = _expr; \
            if (_ret < 0) { \
                return _val; \
            } \
            _ret; \
        })

#define CHECK_TRUE(_expr, _ret) \
        ({ \
            const bool _ok = (bool)(_expr); \
            if (!_ok) { \
                return _ret; \
            } \
        })

#define CHECK_FALSE(_expr, _ret) \
        CHECK_TRUE(!(_expr), _ret)

#define ESP_ERROR_TO_NEGATIVE(_err) \
        (_err > 0 ? -_err : _err)

#define CHECK_ESP(_expr) \
        ({ \
            const auto _ret = _expr; \
            if (_ret != ESP_OK) { \
                return ESP_ERROR_TO_NEGATIVE(_ret); \
            } \
            _ret; \
        })

#define CHECK_ESP_RESULT(_expr, _val) \
        ({ \
            const auto _ret = _expr; \
            if (_ret != ESP_OK) { \
                return _val; \
            } \
            _ret; \
        })

enum Result: int {
    // -100 - 0: Generic result codes
    RESULT_OK = 0,
    RESULT_ERROR = -1,
    RESULT_NO_MEMORY = -2,
    RESULT_BUSY = -3,
    RESULT_INVALID_STATE = -4,
    RESULT_INVALID_PARAM = -5,
    RESULT_NOT_FOUND = -6,
    RESULT_ALREADY_EXIST = -7,
    RESULT_OUT_OF_RANGE = -8,
    RESULT_LIMIT_EXCEEDED = -9,
    RESULT_TOO_SMALL_BUFFER = -10,
    RESULT_TOO_LARGE_DATA = -11,
    RESULT_END_OF_STREAM = -12,
    RESULT_INVALID_FORMAT = -13,
    RESULT_CANCELLED = -14,
    RESULT_ABORTED = -15,
    RESULT_TIMEOUT = -16,
    RESULT_IO_ERROR = -17,
    RESULT_PROTOCOL_ERROR = -18,
    RESULT_INTERNAL_ERROR = -19
};

#endif // !__RESULT_H__
