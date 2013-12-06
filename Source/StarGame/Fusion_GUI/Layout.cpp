#include "stdafx.h"
#include "Layout.h"


using namespace FusionEngine;


void Layout::AddControl(const std::shared_ptr<Control> newControl)
{
	controls.push_back(newControl);
}
std::vector<std::shared_ptr<Control>> Layout::GetControls()
{
	return controls;
}

void Layout::Draw(glutil::MatrixStack &modelMatrix)
{
	for (auto control = controls.begin(); control != controls.end(); ++control)
	{
		(*control)->Draw(modelMatrix);
	}
}