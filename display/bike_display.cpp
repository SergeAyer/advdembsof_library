// Copyright 2022 Haute école d'ingénierie et d'architecture de Fribourg
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/****************************************************************************
 * @file bike_display.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Bike Display header file
 *
 * @date 2023-07-05
 * @version 0.0.1
 ***************************************************************************/

#include "bike_display.hpp"

// DISCO specific
#include "celsius_icon.hpp"
#include "distance_icon.hpp"
#include "fonts.hpp"
#include "gear_icon.hpp"
#include "joystick.hpp"
#include "mbed_trace.h"
#include "mse_logo.hpp"
#include "speedometer_icon.hpp"
#include "thermometer_icon.hpp"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "BikeDisplay"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

namespace advembsof {

// create the LCDDisplay instance as a global variable (not on stack)
static disco::LCDDisplay lcdDisplay;

struct Logos {
    struct ImageInfo {
        const uint32_t* pImageData;
        const uint8_t imageWidth;
        const uint8_t imageHeight;
    };

#if defined(SHOW_WELCOME)
    static constexpr uint8_t kNbrOfImages = 5;
#else
    static constexpr uint8_t kNbrOfImages = 4;
#endif
    static constexpr ImageInfo _imageInfo[kNbrOfImages] = {
        {.pImageData  = speedometer_icon_100,
         .imageWidth  = kSpeedometerIconWidth,
         .imageHeight = kSpeedometerIconHeight},
        {.pImageData  = gear_icon_100,
         .imageWidth  = kGearIconWidth,
         .imageHeight = kGearIconHeight},
        {.pImageData  = thermometer_icon_100,
         .imageWidth  = kThermometerIconWidth,
         .imageHeight = kThermometerIconHeight},
        {.pImageData  = distance_icon_100,
         .imageWidth  = kDistanceIconWidth,
         .imageHeight = kDistanceIconHeight}
#if defined(SHOW_WELCOME)
        ,
        {.pImageData  = mse_logo,
         .imageWidth  = kMSELogoIconWidth,
         .imageHeight = kMSELogoIconHeight}
#endif
    };
};
static const Logos logos;

BikeDisplay::BikeDisplay() {
#if defined(SHOW_WELCOME)
    disco::Joystick::getInstance().setLeftCallback(
        callback(this, &BikeDisplay::onJoystickLeft));
    disco::Joystick::getInstance().setRightCallback(
        callback(this, &BikeDisplay::onJoystickRight));
    disco::Joystick::getInstance().setDownCallback(
        callback(this, &BikeDisplay::onJoystickDown));
    disco::Joystick::getInstance().setUpCallback(
        callback(this, &BikeDisplay::onJoystickUp));
    disco::Joystick::getInstance().setSelCallback(
        callback(this, &BikeDisplay::onJoystickSel));
#endif
}

disco::ReturnCode BikeDisplay::init() {
    // intitialize the LCD display
    disco::ReturnCode displayRC = lcdDisplay.init();
    if (displayRC != disco::ReturnCode::Ok) {
        tr_error("Failed to initialized display");
        return displayRC;
    } else {
        tr_debug("Display initialized");
    }

#if defined(SHOW_WELCOME)
    lcdDisplay.displayWelcome(szWelcomeString, disco::LCDDisplay::AlignMode::CENTER_MODE);
    _logoXPos = kLogoXPos;
    lcdDisplay.displayPicture(
        const_cast<uint32_t*>(logos._imageInfo[kLogoIndex].pImageData),
        _logoXPos,
        kLogoYPos,
        logos._imageInfo[kLogoIndex].imageWidth,
        logos._imageInfo[kLogoIndex].imageHeight);
#endif

    // compute positions
    computePositions();

    // display title
    lcdDisplay.displayTitle("Bike Computer", disco::LCDDisplay::AlignMode::CENTER_MODE);

    // display icons (with lines)
    displayIcons();

    return disco::ReturnCode::Ok;
}

void BikeDisplay::computePositions() {
    // horizontal and vertical lines below title bar
    _vertLineXPos  = lcdDisplay.getWidth() / 2;
    _infoBoxHeight = (lcdDisplay.getHeight() - lcdDisplay.getTitleHeight());
    _horLineYPos   = lcdDisplay.getTitleHeight() + _infoBoxHeight / 2;
    // speed (top left)
    _speedometerIconXPos = kIconXMargin;
    _speedometerIconYPos = _horLineYPos - _infoBoxHeight / 4 -
                           logos._imageInfo[kSpeedometerIndex].imageHeight / 2;
    const uint32_t speedoTextBoxWidth = _vertLineXPos - _speedometerIconXPos;
    _speedometerTextMidXPos =
        _speedometerIconXPos + speedoTextBoxWidth / 2 + kTextXMargin;
    _speedometerTextYPos = _horLineYPos - _infoBoxHeight / 4;
    // distance (bottom left)
    _distanceIconXPos = kIconXMargin;
    _distanceIconYPos = _horLineYPos + _infoBoxHeight / 4 -
                        logos._imageInfo[kDistanceIndex].imageHeight / 2;
    const uint32_t distTextBoxWidth = _vertLineXPos - _distanceIconXPos;
    _distanceTextMidXPos = _distanceIconXPos + distTextBoxWidth / 2 + kTextXMargin;
    _distanceTextYPos    = _horLineYPos + _infoBoxHeight / 4;
    // temperature (top right)
    _temperatureIconXPos = _vertLineXPos + kIconXMargin;
    _temperatureIconYPos = _horLineYPos - _infoBoxHeight / 4 -
                           logos._imageInfo[kTemperatureIndex].imageHeight / 2;
    const uint32_t tempTextBoxWidth = lcdDisplay.getWidth() - _temperatureIconXPos;
    _temperatureTextMidXPos = _temperatureIconXPos + tempTextBoxWidth / 2 + kTextXMargin;
    _temperatureTextYPos    = _horLineYPos - _infoBoxHeight / 4;
    // gear (bottom right)
    _gearIconXPos = _vertLineXPos + kIconXMargin;
    _gearIconYPos =
        _horLineYPos + _infoBoxHeight / 4 - logos._imageInfo[kGearIndex].imageHeight / 2;
    const uint32_t gearTextBoxWidth = lcdDisplay.getWidth() - _gearIconXPos;
    _gearTextMidXPos                = _gearIconXPos + gearTextBoxWidth / 2 + kTextXMargin;
    _gearTextYPos                   = _horLineYPos + _infoBoxHeight / 4;
}

void BikeDisplay::displayIcons() {
    // draw the vertical and horizontal lines
    lcdDisplay.displayVerticalLine(_vertLineXPos, kLineWidth);
    lcdDisplay.displayHorizontalLine(_horLineYPos, kLineWidth);

    // draw the speedometer icon
    lcdDisplay.displayPicture(
        const_cast<uint32_t*>(logos._imageInfo[kSpeedometerIndex].pImageData),
        _speedometerIconXPos,
        _speedometerIconYPos,
        logos._imageInfo[kSpeedometerIndex].imageWidth,
        logos._imageInfo[kSpeedometerIndex].imageHeight);
    // draw the distance icon
    lcdDisplay.displayPicture(
        const_cast<uint32_t*>(logos._imageInfo[kDistanceIndex].pImageData),
        _distanceIconXPos,
        _distanceIconYPos,
        logos._imageInfo[kDistanceIndex].imageWidth,
        logos._imageInfo[kDistanceIndex].imageHeight);

    // draw the temperature icon
    lcdDisplay.displayPicture(
        const_cast<uint32_t*>(logos._imageInfo[kTemperatureIndex].pImageData),
        _temperatureIconXPos,
        _temperatureIconYPos,
        logos._imageInfo[kTemperatureIndex].imageWidth,
        logos._imageInfo[kTemperatureIndex].imageHeight);
    // draw the gear icon
    lcdDisplay.displayPicture(
        const_cast<uint32_t*>(logos._imageInfo[kGearIndex].pImageData),
        _gearIconXPos,
        _gearIconYPos,
        logos._imageInfo[kGearIndex].imageWidth,
        logos._imageInfo[kGearIndex].imageHeight);
}

void BikeDisplay::displayGear(uint8_t gear) {
    char msg[10] = {0};
    int strlen   = snprintf(msg, sizeof(msg), "%d", gear);
    lcdDisplay.setFont(disco::createFont36b());
    const uint32_t msgLen   = strlen * lcdDisplay.getFont()->width;
    const uint32_t textXPos = _gearTextMidXPos - msgLen / 2;
    const uint32_t textYPos = _gearTextYPos - lcdDisplay.getFont()->height / 2;
    lcdDisplay.displayStringAt(
        textXPos, textYPos, msg, disco::LCDDisplay::AlignMode::LEFT_MODE);
    lcdDisplay.refreshLCD();
}

void BikeDisplay::displaySpeed(float speed) {
    char msg[10] = {0};
    int strlen   = snprintf(msg, sizeof(msg), "%.1f km/h", speed);
    lcdDisplay.setFont(disco::createFont18());
    const uint32_t msgLen   = strlen * lcdDisplay.getFont()->width;
    const uint32_t textXPos = _speedometerTextMidXPos - msgLen / 2;
    const uint32_t textYPos = _speedometerTextYPos - lcdDisplay.getFont()->height / 2;
    lcdDisplay.displayStringAt(
        textXPos, textYPos, msg, disco::LCDDisplay::AlignMode::LEFT_MODE);
    lcdDisplay.refreshLCD();
}

void BikeDisplay::displayDistance(float distance) {
    char msg[10] = {0};
    int strlen   = snprintf(msg, sizeof(msg), "%.2f km", distance);
    lcdDisplay.setFont(disco::createFont18());
    const uint32_t msgLen   = strlen * lcdDisplay.getFont()->width;
    const uint32_t textXPos = _distanceTextMidXPos - msgLen / 2;
    const uint32_t textYPos = _distanceTextYPos - lcdDisplay.getFont()->height / 2;
    lcdDisplay.displayStringAt(
        textXPos, textYPos, msg, disco::LCDDisplay::AlignMode::LEFT_MODE);
    lcdDisplay.refreshLCD();
}

void BikeDisplay::displayTemperature(float temperature) {
    char msg[10] = {0};
    int strlen   = snprintf(msg, sizeof(msg), "%.1f", temperature);
    lcdDisplay.setFont(disco::createFont26b());
    const uint32_t msgLen   = strlen * lcdDisplay.getFont()->width;
    const uint32_t textXPos = _temperatureTextMidXPos - msgLen / 2;
    const uint32_t textYPos = _temperatureTextYPos - lcdDisplay.getFont()->height / 2;
    lcdDisplay.displayStringAt(
        textXPos, textYPos, msg, disco::LCDDisplay::AlignMode::LEFT_MODE);
    const uint32_t celsiusIconXPos = textXPos + msgLen;
    const uint32_t celsiusIconYPos = textYPos - kCelsiusIconHeight / 5;
    lcdDisplay.displayPicture(const_cast<uint32_t*>(celsius_icon_32),
                              celsiusIconXPos,
                              celsiusIconYPos,
                              kCelsiusIconWidth,
                              kCelsiusIconHeight);
    lcdDisplay.refreshLCD();
}

void BikeDisplay::reset() {
#if defined(SHOW_WELCOME)
    if (_showWelcome) {
        lcdDisplay.fillDisplay(disco::LCDDisplay::LCD_COLOR_BLACK);
    } else {
        lcdDisplay.displayWelcome(szWelcomeString, _alignMode);
        if (_showPicture) {
            lcdDisplay.displayPicture(
                const_cast<uint32_t*>(_logos._imageInfo[kLogoIndex].pImageData),
                _logoXPos,
                kLogoYPos,
                _logos._imageInfo[kLogoIndex].imageWidth,
                _logos._imageInfo[kLogoIndex].imageHeight);
        }
    }
    _showWelcome = !_showWelcome;
#endif
}

#if defined(SHOW_WELCOME)
void BikeDisplay::onJoystickLeft() {
    _logoXPos  = 0;
    _alignMode = disco::LCDDisplay::AlignMode::LEFT_MODE;
    displayWelcome();
}

void BikeDisplay::onJoystickRight() {
    _logoXPos  = lcdDisplay.getWidth() - _logos._imageInfo[kLogoIndex].imageWidth;
    _alignMode = disco::LCDDisplay::AlignMode::RIGHT_MODE;
    displayWelcome();
}

void BikeDisplay::onJoystickDown() {
    if (_showPicture) {
        _showPicture = false;
        displayWelcome();
    }
}

void BikeDisplay::onJoystickUp() {
    if (!_showPicture) {
        _showPicture = true;
        displayWelcome();
    }
}

void BikeDisplay::onJoystickSel() {
    _logoXPos  = kLogoXPos;
    _alignMode = disco::LCDDisplay::AlignMode::CENTER_MODE;
    displayWelcome();
}

void BikeDisplay::displayWelcome() {
    lcdDisplay.displayWelcome(szWelcomeString, _alignMode);
    if (_showPicture) {
        lcdDisplay.displayPicture(
            const_cast<uint32_t*>(_logos._imageInfo[kLogoIndex].pImageData),
            _logoXPos,
            kLogoYPos,
            _logos._imageInfo[kLogoIndex].imageWidth,
            _logos._imageInfo[kLogoIndex].imageHeight);
    }
}
#endif

}  // namespace advembsof
