#pragma once
#include "Commands/Command.h"
#include "Components/Menu/MenuComponents.h"
#include "GameManager.h"

namespace qbert
{
    class MenuMoveCommand final : public dae::Command
    {
    public:
        MenuMoveCommand(MenuSelectorComponent* pMenu, int direction) : m_pMenu(pMenu), m_Direction(direction) {}
        void Execute() override { m_pMenu->Move(m_Direction); }
    private:
        MenuSelectorComponent* m_pMenu;
        int m_Direction;
    };

    class MenuSelectCommand final : public dae::Command
    {
    public:
        MenuSelectCommand(MenuSelectorComponent* pMenu) : m_pMenu(pMenu) {}
        void Execute() override { m_pMenu->Select(); }
    private:
        MenuSelectorComponent* m_pMenu;
    };

    class StartGameCommand final : public dae::Command
    {
    public:
        StartGameCommand(GameMode mode, int startLevel) : m_Mode(mode), m_StartLevel(startLevel) {}
        void Execute() override
        {
            qbert::GameManager::GetInstance().StartGame(m_Mode, m_StartLevel);
        }
    private:
        GameMode m_Mode;
        int m_StartLevel;
    };
}