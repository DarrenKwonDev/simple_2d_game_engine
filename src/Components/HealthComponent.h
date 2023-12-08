#pragma once

struct HealthComponent {
    int mHealthPercentage;

    HealthComponent(int mHealthPercentage = 0) {
        this->mHealthPercentage = mHealthPercentage;
    }
};