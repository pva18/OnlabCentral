/**
 ***************************************************************************************************
 * @file UiStateMachine.hpp
 * @author Péter Varga
 * @date 2023. 05. 08.
 ***************************************************************************************************
 * @brief This file contains the definition of the UiStateMachine class.
 ***************************************************************************************************
 */

#pragma once

#include <cstdio>
#include <cstdint>
#include <string>
#include <LCD_I2C.h>

#include "AuthenticateList.hpp"
#include "realtime.h"

/**
 * @brief This class represents the state machine of the user interface.
 */
class UiStateMachine
{
public:
    /**
     * @brief This enum represents the buttons of the user interface.
     */
    enum class Button
    {
        NONE,  /**< No button was pressed */
        ENTER, /**< Enter button was pressed */
        BACK,  /**< Back button was pressed */
        LEFT,  /**< Left button was pressed */
        RIGHT  /**< Right button was pressed */
    };

private:
    enum class State
    {
        IDLE,
        SELECT_OPTION_SELECT_ITEM,
        SELECT_OPTION_TIME,
        OPTION_SELECT_ITEM,
        OPTION_TIME,
        VIEW_NAME,
        VIEW_UID,
        VIEW_INTERVAL_START,
        VIEW_INTERVAL_END,
        EDIT_INTERVAL_START,
        EDIT_INTERVAL_END
    };

    State state = State::IDLE;

    const unsigned long millisIdleTimeout = 30000;

    AuthenticateList *authenticate_list;
    LCD_I2C *lcd;

    int selected_item = 0;
    unsigned long millis_last_interaction = 0;
    int editedPart = 0;
    uint32_t editTimeHour;
    uint32_t editTimeMinute;
    bool editStarted = false;

public:
    /**
     * @brief Construct a new Ui State Machine object.
     * @param authenticate_list List that contains the authetication data
     * @param lcd LCD display
     */
    UiStateMachine(AuthenticateList *authenticate_list, LCD_I2C *lcd)
        : authenticate_list(authenticate_list), lcd(lcd)
    {
    }

    /**
     * @brief Update the state machine.
     * @param button Button that was pressed
     * @param millis_current Current time in milliseconds
     */
    void update(Button button, unsigned long millis_current)
    {
        if (button != Button::NONE)
        {
            lcd->backlight();
            millis_last_interaction = millis_current;
        }
        else if (millis_current - millis_last_interaction > millisIdleTimeout)
        {
            lcd->noBacklight();
        }

        switch (state)
        {
        case State::IDLE:
            displayTitle();
            if (button == Button::ENTER)
            {
                state = State::SELECT_OPTION_SELECT_ITEM;
            }
            break;

        case State::SELECT_OPTION_SELECT_ITEM:
            displaySelectOptionSelectItem();

            switch (button)
            {
            case Button::ENTER:
                state = State::OPTION_SELECT_ITEM;
                break;

            case Button::RIGHT:
                state = State::SELECT_OPTION_TIME;
                break;

            case Button::LEFT:
                state = State::SELECT_OPTION_TIME;
                break;
            case Button::BACK:
                state = State::IDLE;
                break;
            }
            break;

        case State::SELECT_OPTION_TIME:
            displaySelectOptionTime();

            switch (button)
            {
            case Button::ENTER:
                state = State::OPTION_TIME;
                break;

            case Button::RIGHT:
                state = State::SELECT_OPTION_SELECT_ITEM;
                break;

            case Button::LEFT:
                state = State::SELECT_OPTION_SELECT_ITEM;
                break;
            case Button::BACK:
                state = State::IDLE;
                break;
            }
            break;

        case State::OPTION_TIME:
            if (!editStarted)
            {
                editStarted = true; // TODO
                uint32_t time = REALTIME_Get();
                editTimeHour = time % (60 * 60 * 24) / (60 * 60);
                editTimeMinute = time % (60 * 60) / 60;
            }

            displayOptionTime(editedPart);

            switch (button)
            {
            case Button::ENTER:
                editedPart == 1 ? editedPart = 0 : editedPart = 1;
                break;

            case Button::RIGHT:
                if (editedPart == 0)
                {
                    editTimeHour = (editTimeHour + 1) % 24;
                }
                else
                {
                    editTimeMinute = (editTimeMinute + 1) % 60;
                }
                break;

            case Button::LEFT:
                if (editedPart == 0)
                {
                    editTimeHour = (editTimeHour - 1) % 24;
                }
                else
                {
                    editTimeMinute = (editTimeMinute - 1) % 60;
                }

                break;

            case Button::BACK:
                REALTIME_Set(editTimeHour * (60 * 60) + editTimeMinute * 60);

                editStarted = false;
                state = State::SELECT_OPTION_TIME;
                break;
            }
            break;

        case State::OPTION_SELECT_ITEM:
            displayList(selected_item);

            switch (button)
            {
            case Button::ENTER:
                if (authenticate_list->size() > 0)
                {
                    state = State::VIEW_NAME;
                }
                break;

            case Button::RIGHT:
                incrementSelected();
                break;

            case Button::LEFT:
                decrementSelected();
                break;

            case Button::BACK:
                state = State::SELECT_OPTION_SELECT_ITEM;
                break;
            }
            break;

        case State::VIEW_NAME:
            displayName(selected_item);

            switch (button)
            {
            case Button::RIGHT:
                state = State::VIEW_UID;
                break;

            case Button::LEFT:
                state = State::VIEW_INTERVAL_END;
                break;

            case Button::BACK:
                state = State::OPTION_SELECT_ITEM;
                break;
            }
            break;

        case State::VIEW_UID:
            displayUid(selected_item);

            switch (button)
            {
            case Button::RIGHT:
                state = State::VIEW_INTERVAL_START;
                break;

            case Button::LEFT:
                state = State::VIEW_NAME;
                break;

            case Button::BACK:
                state = State::OPTION_SELECT_ITEM;
                break;
            }
            break;

        case State::VIEW_INTERVAL_START:
            displayIntervalStart(selected_item);

            switch (button)
            {
            case Button::RIGHT:
                state = State::VIEW_INTERVAL_END;
                break;

            case Button::LEFT:
                state = State::VIEW_UID;
                break;

            case Button::ENTER:
                state = State::EDIT_INTERVAL_START;
                break;

            case Button::BACK:
                state = State::OPTION_SELECT_ITEM;
                break;
            }
            break;

        case State::VIEW_INTERVAL_END:
            displayIntervalEnd(selected_item);

            switch (button)
            {
            case Button::RIGHT:
                state = State::VIEW_NAME;
                break;

            case Button::LEFT:
                state = State::VIEW_INTERVAL_START;
                break;

            case Button::ENTER:
                state = State::EDIT_INTERVAL_END;
                break;

            case Button::BACK:
                state = State::OPTION_SELECT_ITEM;
                break;
            }
            break;

        case State::EDIT_INTERVAL_START:
            if (!editStarted)
            {
                editStarted = true;
                editTimeHour = (*authenticate_list)[selected_item]->getIntervalStart() % (60 * 60 * 24) / (60 * 60);
                editTimeMinute = (*authenticate_list)[selected_item]->getIntervalStart() % (60 * 60) / 60;
            }

            displayEditIntervalStart(selected_item, editedPart);

            switch (button)
            {
            case Button::ENTER:
                editedPart == 1 ? editedPart = 0 : editedPart = 1;
                break;

            case Button::RIGHT:
                if (editedPart == 0)
                {
                    editTimeHour = (editTimeHour + 1) % 24;
                }
                else
                {
                    editTimeMinute = (editTimeMinute + 1) % 60;
                }
                break;

            case Button::LEFT:
                if (editedPart == 0)
                {
                    editTimeHour = (editTimeHour - 1) % 24;
                }
                else
                {
                    editTimeMinute = (editTimeMinute - 1) % 60;
                }

                break;

            case Button::BACK:
                (*authenticate_list)[selected_item]->setIntervalStart(editTimeHour * (60 * 60) + editTimeMinute * 60);

                editStarted = false;
                state = State::VIEW_INTERVAL_START;
                break;
            }
            break;

        case State::EDIT_INTERVAL_END:
            if (!editStarted)
            {
                editStarted = true;
                editTimeHour = (*authenticate_list)[selected_item]->getIntervalEnd() % (60 * 60 * 24) / (60 * 60);
                editTimeMinute = (*authenticate_list)[selected_item]->getIntervalEnd() % (60 * 60) / 60;
            }

            displayEditIntervalEnd(selected_item, editedPart);

            switch (button)
            {
            case Button::ENTER:
                editedPart == 1 ? editedPart = 0 : editedPart = 1;
                break;

            case Button::RIGHT:
                if (editedPart == 0)
                {
                    editTimeHour = (editTimeHour + 1) % 24;
                }
                else
                {
                    editTimeMinute = (editTimeMinute + 1) % 60;
                }
                break;

            case Button::LEFT:
                if (editedPart == 0)
                {
                    editTimeHour = (editTimeHour - 1) % 24;
                }
                else
                {
                    editTimeMinute = (editTimeMinute - 1) % 60;
                }

                break;

            case Button::BACK:
                (*authenticate_list)[selected_item]->setIntervalEnd(editTimeHour * (60 * 60) + editTimeMinute * 60);

                editStarted = false;
                state = State::VIEW_INTERVAL_END;
                break;
            }
            break;
        }
    }

private:
    bool millisIsTimeout(Button button, unsigned long millis_current)
    {
        if (button != Button::NONE)
        {
            millis_last_interaction = millis_current;
            return false;
        }
        return ((millis_current - millis_last_interaction) > millisIdleTimeout);
    }

    void incrementSelected(void)
    {
        selected_item++;
        if (selected_item >= (*authenticate_list).size())
        {
            selected_item = 0;
        }
    }

    void decrementSelected(void)
    {
        selected_item--;
        if (selected_item < 0)
        {
            selected_item = (*authenticate_list).size() - 1;
        }
    }

    void displayTitle(void)
    {
        lcd->clear();
        lcd->setCursor(0, 0);
        lcd->print("Belepteto");
        lcd->setCursor(0, 7);
        lcd->print("Kozponti");
    }

    void displaySelectOptionSelectItem(void)
    {
        lcd->clear();
        lcd->setCursor(0, 0);
        lcd->print("Elem kivalasztasa");
    }

    void displaySelectOptionTime(void)
    {
        char display_str[17];
        uint32_t time = REALTIME_Get();
        uint32_t timeHour = time % (60 * 60 * 24) / (60 * 60);
        uint32_t timeMinute = time % (60 * 60) / 60;
        sprintf(display_str, "%02d:%02d", timeHour, timeMinute);

        lcd->clear();
        lcd->setCursor(0, 0);
        lcd->print("Ido");
        lcd->setCursor(0, 1);
        lcd->print(display_str);
    }

    void displayOptionTime(int selected_part)
    {
        char display_str[17];
        if (selected_part == 0)
        {
            sprintf(display_str, "*%02d*:%02d", editTimeHour, editTimeMinute);
        }
        else if (selected_part == 1)
        {
            sprintf(display_str, "%02d:*%02d*", editTimeHour, editTimeMinute);
        }
        else
        {
            sprintf(display_str, "00:00");
        }

        lcd->clear();
        lcd->setCursor(0, 0);
        lcd->print("Ido: ");
        lcd->setCursor(0, 1);
        lcd->print(display_str);
    }

    void displayList(int selected_item)
    {
        if ((*authenticate_list).size() < 1)
        {
            lcd->clear();
            lcd->setCursor(0, 0);
            lcd->print("Ures lista");
            return;
        }
        const AuthenticateData *item = (*authenticate_list)[selected_item];
        lcd->clear();
        lcd->setCursor(0, 0);
        lcd->print("Lista elem:");
        lcd->setCursor(0, 1);
        lcd->print(item->getName());
    }

    void displayName(int selected_item)
    {
        const AuthenticateData *item = (*authenticate_list)[selected_item];
        lcd->clear();
        lcd->setCursor(0, 0);
        lcd->print("Nev: ");
        lcd->setCursor(0, 1);
        lcd->print(item->getName());
    }

    void displayUid(int selected_item)
    {
        const AuthenticateData *item = (*authenticate_list)[selected_item];
        const uint8_t *uid = item->getUid();

        char display_str0[17];
        char display_str1[17];
        sprintf(display_str0, "UID: 0x%02X%02X %02X%02X", uid[0], uid[1], uid[2], uid[3]);
        sprintf(display_str1, "  %02X%02X %02X%02X %02X%02X", uid[4], uid[5], uid[6], uid[7], uid[8], uid[9]);

        lcd->clear();
        lcd->setCursor(0, 0);
        lcd->print(display_str0);
        lcd->setCursor(0, 1);
        lcd->print(display_str1);
    }

    void displayIntervalStart(int selected_item)
    {
        const AuthenticateData *item = (*authenticate_list)[selected_item];
        uint32_t interval_start = item->getIntervalStart();
        uint32_t hour = interval_start % (60 * 60 * 24) / (60 * 60);
        uint32_t minute = interval_start % (60 * 60) / 60;

        char display_str[17];
        sprintf(display_str, "%02d:%02d", hour, minute);

        lcd->clear();
        lcd->setCursor(0, 0);
        lcd->print("Kezdete: ");
        lcd->setCursor(0, 1);
        lcd->print(display_str);
    }

    void displayIntervalEnd(int selected_item)
    {
        const AuthenticateData *item = (*authenticate_list)[selected_item];
        uint32_t interval_end = item->getIntervalEnd();
        uint32_t hour = interval_end % (60 * 60 * 24) / (60 * 60);
        uint32_t minute = interval_end % (60 * 60) / 60;

        char display_str[17];
        sprintf(display_str, "%02d:%02d", hour, minute);

        lcd->clear();
        lcd->setCursor(0, 0);
        lcd->print("Vege: ");
        lcd->setCursor(0, 1);
        lcd->print(display_str);
    }

    void displayEditIntervalStart(int selected_item, int selected_part)
    {
        char display_str[17];
        if (selected_part == 0)
        {
            sprintf(display_str, "*%02d*:%02d", editTimeHour, editTimeMinute);
        }
        else if (selected_part == 1)
        {
            sprintf(display_str, "%02d:*%02d*", editTimeHour, editTimeMinute);
        }
        else
        {
            sprintf(display_str, "00:00");
        }

        lcd->clear();
        lcd->setCursor(0, 0);
        lcd->print("Kezdete: ");
        lcd->setCursor(0, 1);
        lcd->print(display_str);
    }

    void displayEditIntervalEnd(int selected_item, int selected_part)
    {
        char display_str[17];
        if (selected_part == 0)
        {
            sprintf(display_str, "*%02d*:%02d", editTimeHour, editTimeMinute);
        }
        else if (selected_part == 1)
        {
            sprintf(display_str, "%02d:*%02d*", editTimeHour, editTimeMinute);
        }
        else
        {
            sprintf(display_str, "00:00");
        }

        lcd->clear();
        lcd->setCursor(0, 0);
        lcd->print("Vege: ");
        lcd->setCursor(0, 1);
        lcd->print(display_str);
    }
}; /* class UiStateMachine */
