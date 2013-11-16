#include "stdafx.h"
#include "Control.h"


using namespace FusionEngine;


void Control::Init(const std::string &backgroundImageFileName)
{
	background.Init(backgroundImageFileName);
}

void Control::Draw(glutil::MatrixStack &modelMatrix)
{
	glutil::MatrixStack identityMatrix;
	identityMatrix.SetIdentity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	background.Draw(identityMatrix);

	glDisable(GL_BLEND);
}