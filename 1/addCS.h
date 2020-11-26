#pragma once
#include "BasicEffect.h"
class addCS :
    public PDEeffect
{
    // Í¨¹ý PDEeffect ¼Ì³Ð
    virtual void Draw() override;
    virtual void LoadFx() override;
    virtual void SetTopoLayout() override;
};

