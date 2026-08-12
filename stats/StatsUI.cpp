#include "StatsUI.h"

#include <imgui.h>

#include <array>
#include <algorithm>
#include <cstddef>

namespace
{
    constexpr std::size_t HISTORY_SIZE =
        ConfigConstexpr::statsHistorySize;


    struct History
    {
        std::array<float, HISTORY_SIZE> ticks{};
        std::array<float, HISTORY_SIZE> humanTicks{};
        std::array<float, HISTORY_SIZE> fps{};

        std::size_t index = 0;
        std::size_t count = 0;


        void add(const Stats::Data& data)
        {
            ticks[index] =
                static_cast<float>(
                    data.ticksPerSecond);

            humanTicks[index] =
                static_cast<float>(
                    data.humanTicksPerSecond);

            fps[index] =
                static_cast<float>(
                    data.FPS);


            index =
                (index + 1) % HISTORY_SIZE;


            if (count < HISTORY_SIZE)
                ++count;
        }
    };


    History history;


    void drawPlot(
        const char* label,
        const std::array<float, HISTORY_SIZE>& values,
        std::size_t index,
        std::size_t count)
    {
        if (count == 0)
            return;


        std::array<float, HISTORY_SIZE> ordered{};


        for (std::size_t i = 0; i < count; ++i)
        {
            const std::size_t position =
                (index + HISTORY_SIZE - count + i)
                % HISTORY_SIZE;

            ordered[i] = values[position];
        }


        float maxValue = 1.0f;


        for (std::size_t i = 0; i < count; ++i)
        {
            maxValue =
                std::max(
                    maxValue,
                    ordered[i]);
        }


        ImGui::PlotLines(
            label,
            ordered.data(),
            static_cast<int>(count),
            0,
            nullptr,
            0.0f,
            maxValue * 1.1f,
            ImVec2(-1.0f, 80.0f)
        );
    }
}


void StatsUI::draw(const Stats::Data& data)
{
    // ============================================================
    // UPDATE HISTORY
    // ============================================================

    history.add(data);


    // ============================================================
    // PERFORMANCE WINDOW
    // ============================================================

    ImGui::SetNextWindowPos(
        ImVec2(
            ImGui::GetIO().DisplaySize.x - 10.0f,
            10.0f
        ),
        ImGuiCond_Always,
        ImVec2(1.0f, 0.0f)
    );


    ImGui::SetNextWindowSize(
        ImVec2(350.0f, 0.0f),
        ImGuiCond_Always
    );


    ImGui::Begin(
        "Simulation Performance",
        nullptr,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize
    );


    ImGui::Text(
        "Ticks per second: %d",
        data.ticksPerSecond
    );


    ImGui::Text(
        "Human ticks per second: %d",
        data.humanTicksPerSecond
    );


    ImGui::Text(
        "FPS: %d",
        data.FPS
    );


    ImGui::Separator();


    ImGui::Text(
        "Total ticks: %llu",
        static_cast<unsigned long long>(
            data.totalTicks)
    );


    ImGui::Text(
        "Total human ticks: %llu",
        static_cast<unsigned long long>(
            data.totalHumanTicks)
    );


    ImGui::Separator();


    if (ImGui::CollapsingHeader(
            "Performance Graphs",
            ImGuiTreeNodeFlags_DefaultOpen))
    {
        drawPlot(
            "Ticks / s",
            history.ticks,
            history.index,
            history.count
        );


        drawPlot(
            "Human ticks / s",
            history.humanTicks,
            history.index,
            history.count
        );


        drawPlot(
            "FPS",
            history.fps,
            history.index,
            history.count
        );
    }


    const float performanceWindowHeight =
        ImGui::GetWindowSize().y;


    ImGui::End();


    // ============================================================
    // SIMULATION STATISTICS WINDOW
    // ============================================================

    ImGui::SetNextWindowPos(
        ImVec2(
            ImGui::GetIO().DisplaySize.x - 10.0f,
            10.0f +
                performanceWindowHeight +
                10.0f
        ),
        ImGuiCond_Always,
        ImVec2(1.0f, 0.0f)
    );


    ImGui::SetNextWindowSize(
        ImVec2(350.0f, 0.0f),
        ImGuiCond_Always
    );


    ImGui::Begin(
        "Simulation Statistics",
        nullptr,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize
    );


    // ============================================================
    // WORLD RESOURCES
    // ============================================================

    if (ImGui::CollapsingHeader(
            "World Resources",
            ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text(
            "Food on map: %zu",
            data.foodCount
        );

        ImGui::Text(
            "Trees on map: %zu",
            data.treeCount
        );

        ImGui::Text(
            "Stone on map: %zu",
            data.stoneCount
        );
    }


    // ============================================================
    // HUMANS
    // ============================================================

    if (ImGui::CollapsingHeader(
            "Humans",
            ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text(
            "Total humans: %zu",
            data.humansCount
        );

        ImGui::Text(
            "Humans on map: %zu",
            data.humansOnMap
        );

        ImGui::Text(
            "Humans with house: %zu",
            data.humansHavingHouse
        );


        ImGui::Separator();


        ImGui::Text(
            "Food collectors: %zu",
            data.foodCollectors
        );

        ImGui::Text(
            "Wood collectors: %zu",
            data.woodCollectors
        );

        ImGui::Text(
            "Stone collectors: %zu",
            data.stoneCollectors
        );

        ImGui::Text(
            "Builders: %zu",
            data.builders
        );

        ImGui::Text(
            "Assigned: %zu",
            data.assigned
        );
    }


    // ============================================================
    // WORKERS
    // ============================================================

    if (ImGui::CollapsingHeader("Workers"))
    {
        ImGui::Text(
            "Farm workers: %zu",
            data.farmWorkers
        );

        ImGui::Text(
            "Sawmill workers: %zu",
            data.sawmillWorkers
        );

        ImGui::Text(
            "Mine workers: %zu",
            data.mineWorkers
        );
    }


    // ============================================================
    // CIVILIZATION RESOURCES
    // ============================================================

    if (ImGui::CollapsingHeader(
            "Civilization Resources",
            ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text(
            "Food: %lld",
            static_cast<long long>(
                data.food)
        );

        ImGui::Text(
            "Wood: %lld",
            static_cast<long long>(
                data.wood)
        );

        ImGui::Text(
            "Stone: %lld",
            static_cast<long long>(
                data.stone)
        );

        ImGui::Text(
            "chances left: %lld",
                data.civilizationChancesLeft
        );
    }


    // ============================================================
    // BUILDINGS
    // ============================================================

    if (ImGui::CollapsingHeader(
            "Buildings",
            ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text(
            "Houses: %zu",
            data.houses
        );

        ImGui::Text(
            "Farms: %zu",
            data.farms
        );

        ImGui::Text(
            "Sawmills: %zu",
            data.sawmills
        );

        ImGui::Text(
            "Mines: %zu",
            data.mines
        );


        ImGui::Separator();


        ImGui::Text(
            "Houses under construction: %zu",
            data.housesUnderConstruction
        );

        ImGui::Text(
            "Farms under construction: %zu",
            data.farmsUnderConstruction
        );

        ImGui::Text(
            "Sawmills under construction: %zu",
            data.sawmillsUnderConstruction
        );

        ImGui::Text(
            "Mines under construction: %zu",
            data.minesUnderConstruction
        );
    }


    ImGui::End();
}