#pragma once
#include "BasicEffect.h"
class addCS :
    public PDEeffect
{
    // ͨ�� PDEeffect �̳�
    virtual void Draw() override;
    virtual void LoadFx() override;
    virtual void SetTopoLayout() override;
};

