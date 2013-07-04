//Copyright 2012, 2013 Tsvetan Tsvetanov
//This file is part of the Star Game.
//
//The Star Game is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//The Star Game is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with the Star Game.  If not, see <http://www.gnu.org/licenses/>.


#include "stdafx.h"
#include "AssetLoader.h"

#include "../framework/ErrorAPI.h"

using namespace FusionEngine;

/////////////////////
///  Mesh Loader  ///
/////////////////////
MeshEntry::~MeshEntry()
{
	if(vertexBuffer != -1)
    {
        glDeleteBuffers(1, &vertexBuffer);
    }
    if(indexBuffer != -1)
    {
        glDeleteBuffers(1, &indexBuffer);
    }
}

void MeshEntry::Init(const std::vector<Vertex> &vertices,
					 const std::vector<unsigned int> &indices)
{
	indicesCount = indices.size();

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indicesCount, &indices[0], GL_STATIC_DRAW);
}


MeshAssetObject MeshLoader::Load(const std::string &type, const std::string &name)
{
	MeshAssetObject loadedMesh;

	Assimp::Importer importer;

	const aiScene *scene = importer.ReadFile("../data/" + type + "/" + name, 
											 aiProcess_Triangulate | aiProcess_GenNormals);

	if(!scene)
    {
		std::string errorMessage = "cannot parse ";
        errorMessage += type + "/" + name; 
        errorMessage += ": ";
        errorMessage += importer.GetErrorString();
        HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
    }
    else 
    {
		InitFromScene(scene, loadedMesh);
    }

	return loadedMesh;
}

void MeshLoader::InitFromScene(const aiScene *scene, MeshAssetObject &loadedMesh)
{
	for(unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
		const aiMesh *mesh = scene->mMeshes[i];
		InitMesh(i, mesh, loadedMesh);
    }

	InitMaterials(scene, loadedMesh);
}

void MeshLoader::InitMesh(unsigned int index, const aiMesh *mesh, MeshAssetObject &loadedMesh)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	const aiVector3D zeroVector(0.0f, 0.0f, 0.0f);

	for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
		const aiVector3D *position = &(mesh->mVertices[i]);
		const aiVector3D *normal = &(mesh->mNormals[i]);
		const aiVector3D *textureCoordinate = 
			mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][i]) : &zeroVector;

		Vertex vert(glm::vec3(position->x, position->y, position->z),
					glm::vec2(textureCoordinate->x, textureCoordinate->y),
					glm::vec3(normal->x, normal->y, normal->z));
		vertices.push_back(vert);
    }

	for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
		const aiFace &face = mesh->mFaces[i];

		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
    }

	MeshEntry newMeshEntry = MeshEntry(mesh->mMaterialIndex);
	newMeshEntry.Init(vertices, indices);

	loadedMesh.AddEntry(newMeshEntry);
}

void MeshLoader::InitMaterials(const aiScene *scene, MeshAssetObject &loadedMesh)
{
	for(unsigned int i = 0; i < scene->mNumMaterials; i++)
    {
		const aiMaterial *material = scene->mMaterials[i];

		std::shared_ptr<Texture2D> newTexture;

		if(material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
			aiString path;

			if(material->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
            {
				if(!newTexture->Load(path.C_Str(), GL_RGB, GL_RGB, GL_UNSIGNED_BYTE))
                {
					HandleUnexpectedError("loading texture " + std::string(path.C_Str()), __LINE__, __FILE__);
					return;
                }
            }
        }

		// Load a white texture in case the model doesn't include its own
		if(!newTexture)
        {
			newTexture = std::shared_ptr<Texture2D>(new Texture2D());
			std::string pathToWhiteTexture = "../data/mesh-files/white.png";
			if(!newTexture->Load(pathToWhiteTexture, GL_RGB, GL_BGR, GL_UNSIGNED_BYTE))
            {
				HandleUnexpectedError("loading DEFAULT texture from path " + pathToWhiteTexture, __LINE__, __FILE__);
				return;
            }
        }

		loadedMesh.AddTexture(newTexture);
    }
}


void MeshAssetObject::AddEntry(const MeshEntry &newMeshEntry)
{
	meshEntries.push_back(newMeshEntry);
}
void MeshAssetObject::AddTexture(const std::shared_ptr<Texture2D>& newTexture) 
{
	textures.push_back(newTexture);
}

const std::vector<MeshEntry> MeshAssetObject::GetMeshEntries()
{
	return meshEntries;
}
const std::vector<std::shared_ptr<Texture2D>> MeshAssetObject::GetTextures()
{
	return textures;
}


//////////////////////
///  Image Loader  ///
//////////////////////
const std::shared_ptr<Texture2D> ImageAssetObject::GetLoadedTexture()
{
	return loadedTexture;
}


ImageAssetObject ImageLoader::Load(const std::string &type, const std::string &name)
{
	std::shared_ptr<Texture2D> loadedTexture;

	loadedTexture = std::shared_ptr<Texture2D>(new Texture2D());
	if(!loadedTexture->Load("../data/" + type + "/" + name, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE))
    {
		HandleUnexpectedError("cannot load texture " + type + "/" + name, __LINE__, __FILE__);
    }

	return ImageAssetObject(loadedTexture);
}


//////////////////////
///  Audio Loader  ///
//////////////////////
const std::vector<std::pair<ChannelType, AudioData>> AudioAssetObject::GetAllLoadedAudios()
{
	return loadedAudio;
}


AudioAssetObject CAudioLoader::Load(const std::string &type, const std::string &name)
{
	YAML::Node audio = YAML::LoadFile("../data/" + type + "/" + name);

	AudioData audioData;

	std::vector<std::pair<ChannelType, AudioData>> loadedAudio;

	for(YAML::Node::const_iterator channel = audio.begin();
		channel != audio.end(); ++channel)
    {
		if(channel->first.as<std::string>() == "channel-master")
        {
			audioData.channel = CHANNEL_MASTER;
        }
        else if(channel->first.as<std::string>() == "channel-interaction")
        {
			audioData.channel = CHANNEL_INTERACTION;
        }
        else if(channel->first.as<std::string>() == "channel-game")
        {
			audioData.channel = CHANNEL_GAME;
        }
		audioData.channelVolume = channel->second["volume"].as<float>();

		AudioFile audioFile;
		for(YAML::Node::const_iterator music = channel->second["music"].begin();
			music != channel->second["music"].end(); ++music)
        {
			audioFile.soundType = SoundType(music->second["sound-type"].as<int>());
			audioFile.path = music->second["sound-file"].as<std::string>();
            if(music->second["sound-loop"])
            {
				audioFile.isLooping = true;
            }
            else
            {
				audioFile.isLooping = false;
            }
			audioData.audioFiles.push_back(audioFile);
        }

		loadedAudio.push_back(std::pair<ChannelType, AudioData>(audioData.channel, audioData));
    }

	return AudioAssetObject(loadedAudio);
}


////////////////////
///  GUI Loader  ///
////////////////////
std::shared_ptr<Layout> GUIAssetObject::GetLayout(LayoutType layoutType)
{
	if(layouts.find(layoutType) == layouts.end())
    {
        std::string errorMessage = "no layout of type ";
        switch(layoutType)
        {
        case LAYOUT_IN_GAME:
            errorMessage += "LAYOUT_IN_GAME";
            break;
        case LAYOUT_LOAD_GAME:
            errorMessage += "LAYOUT_LOAD_GAME";
            break;
        case LAYOUT_MENU:
            errorMessage += "LAYOUT_MENU";
            break;
        case LAYOUT_NEW_GAME:
            errorMessage += "LAYOUT_NEW_GAME";
            break;
        case LAYOUT_OPTIONS:
            errorMessage += "LAYOUT_OPTIONS";
            break;
        case LAYOUT_OTHER:
            errorMessage += "LAYOUT_OTHER";
            break;
        case LAYOUT_SAVE_GAME:
            errorMessage += "LAYOUT_SAVE_GAME";
            break;
        default:
            HandleUnexpectedError("unexpected \'default\' reached", __LINE__, __FILE__);
            break;
        }
        HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
    }

    return layouts[layoutType];
}

const std::map<LayoutType, std::shared_ptr<Layout>> &GUIAssetObject::GetAllLoadedLayouts()
{
	return layouts;
}


GUIAssetObject GUILoader::Load(const std::string &type, const std::string &name)
{
	// TODO: Figure out how to pass them
	int windowWidth = 800;
	int windowHeight = 600;


	std::string fontsDir = "";
    std::string texturesDir = "";
    std::string defaultFont = "";


    YAML::Node guiData = YAML::LoadFile("../data/" + type + "/" + name);

    LayoutData layoutData;

    fontsDir = guiData["details"]["fonts-dir"].as<std::string>();
    texturesDir = guiData["details"]["textures-dir"].as<std::string>();
    defaultFont = guiData["details"]["default-font"].as<std::string>();
    
	std::map<LayoutType, std::shared_ptr<Layout>> layouts;
    ControlData controlData;
    if(guiData["details"]["global-on-hover"])
    {
        for(YAML::Node::const_iterator onHoverProp = guiData["details"]["global-on-hover"].begin();
            onHoverProp != guiData["details"]["global-on-hover"].end(); ++onHoverProp)
        {
            if(onHoverProp->first.as<std::string>() == "left-background-image")
            {
				controlData.onHoverLeftBackgroundImage = texturesDir;
                controlData.onHoverLeftBackgroundImage += onHoverProp->second.as<std::string>();
            }
            if(onHoverProp->first.as<std::string>() == "right-background-image")
            {
				controlData.onHoverRightBackgroundImage = texturesDir;
				controlData.onHoverRightBackgroundImage += onHoverProp->second.as<std::string>();
            }
			if(onHoverProp->first.as<std::string>() == "middle-background-image")
            {
				controlData.onHoverMiddleBackgroundImage = texturesDir;
				controlData.onHoverMiddleBackgroundImage += onHoverProp->second.as<std::string>();
            }
            if(onHoverProp->first.as<std::string>() == "font-color")
            {
                controlData.onHoverFontColor = glm::vec4(onHoverProp->second[0].as<float>(),
                                                         onHoverProp->second[1].as<float>(),
                                                         onHoverProp->second[2].as<float>(),
                                                         onHoverProp->second[3].as<float>());
            }
            if(onHoverProp->first.as<std::string>() == "font")
            {
                controlData.onHoverFont = fontsDir + onHoverProp->second.as<std::string>();
            }
            if(onHoverProp->first.as<std::string>() == "text")
            {
                controlData.onHoverText = onHoverProp->second.as<std::string>();
            }
        }
    }

    for(YAML::Node::const_iterator guiNode = guiData.begin();
        guiNode != guiData.end(); ++guiNode)
    {
        if(guiNode->first.as<std::string>() == "layout-game")
        {
            layoutData.layoutType = LAYOUT_IN_GAME;
        }
        else if(guiNode->first.as<std::string>() == "layout-menu")
        {
            layoutData.layoutType = LAYOUT_MENU;
        }
        else if(guiNode->first.as<std::string>() == "layout-save-game")
        {
            layoutData.layoutType = LAYOUT_SAVE_GAME;
        }
        else if(guiNode->first.as<std::string>() == "layout-load-game")
        {
            layoutData.layoutType = LAYOUT_LOAD_GAME;
        }
        else if(guiNode->first.as<std::string>() == "layout-options")
        {
            layoutData.layoutType = LAYOUT_OPTIONS;
        }
        
        if(guiNode->first.as<std::string>() != "details")
        {
            layoutData.layoutColor = glm::vec4(guiNode->second["background-color"][0].as<float>(),
                                               guiNode->second["background-color"][1].as<float>(), 
                                               guiNode->second["background-color"][2].as<float>(), 
                                               guiNode->second["background-color"][3].as<float>());
            layoutData.layoutIsSet = guiNode->second["is-active"].as<bool>();

            std::shared_ptr<Layout> newLayout =
                std::shared_ptr<Layout>(new Layout(layoutData.layoutType, layoutData.layoutColor));
            layouts.insert(std::make_pair(layoutData.layoutType, newLayout));

            layouts[layoutData.layoutType]->Set(layoutData.layoutIsSet);
            if(guiNode->second["background-image"].IsDefined())
            {
                layoutData.layoutBackgroundImageFileName = texturesDir;
                layoutData.layoutBackgroundImageFileName += guiNode->second["background-image"].as<std::string>();
                layouts[layoutData.layoutType]->SetBackgroundImage(1280, 768, layoutData.layoutBackgroundImageFileName);
            }

            if(guiNode->second["controls"])
            {
                for(YAML::Node::const_iterator control = guiNode->second["controls"].begin();
                    control != guiNode->second["controls"].end(); ++control)
                {
                    if(control->second["control-type"].as<std::string>() == "label")
                    {					
                        controlData.name = control->second["name"].as<std::string>();
                        controlData.text = control->second["text"].as<std::string>();
                        controlData.hasBackground = control->second["has-background"].as<bool>();
                        controlData.isVisible = control->second["is-visible"].as<bool>();
                        controlData.isUsingPercentage = control->second["is-using-percentage"].as<bool>();
                        controlData.fontColor = glm::vec4(control->second["font-color"][0].as<float>(),
                                                          control->second["font-color"][1].as<float>(),
                                                          control->second["font-color"][2].as<float>(),
                                                          control->second["font-color"][3].as<float>());
                        controlData.textSize = control->second["text-size"].as<int>();
                        controlData.leftBackgroundImage = "";
                        controlData.rightBackgroundImage = "";
                        controlData.middleBackgroundImage = "";
                        if(controlData.hasBackground)
                        {
                            controlData.margins = glm::vec4(control->second["margins"][0].as<float>(),
                                                            control->second["margins"][1].as<float>(),
                                                            control->second["margins"][2].as<float>(),
                                                            control->second["margins"][3].as<float>());

                            controlData.leftBackgroundImage = texturesDir;
                            controlData.leftBackgroundImage += control->second["bckg-left"].as<std::string>();
							controlData.rightBackgroundImage = texturesDir;
							controlData.rightBackgroundImage += control->second["bckg-right"].as<std::string>();
							controlData.middleBackgroundImage = texturesDir;
							controlData.middleBackgroundImage += control->second["bckg-middle"].as<std::string>();
                        }
                        controlData.percentagedPosition = glm::vec2();
                        controlData.position = glm::vec2();
                        if(controlData.isUsingPercentage)
                        {
                            controlData.percentagedPosition = 
                                glm::vec2(control->second["percentaged-pos"][0].as<float>(),
                                          control->second["percentaged-pos"][1].as<float>());
                        }
                        else
                        {
                            controlData.position = glm::vec2(control->second["position"][0].as<float>(),
                                                             control->second["position"][1].as<float>());
                        }

                        HoveredProperties newHoveredProps;
                        newHoveredProps.backgroundLeftImage = controlData.onHoverLeftBackgroundImage;
						newHoveredProps.backgroundRightImage = controlData.onHoverRightBackgroundImage;
						newHoveredProps.backgroundMiddleImage = controlData.onHoverMiddleBackgroundImage;
                        newHoveredProps.text = controlData.onHoverText;
                        newHoveredProps.textColor = controlData.onHoverFontColor;
                        newHoveredProps.font = controlData.onHoverFont;

                        std::shared_ptr<Label> label =
                            std::shared_ptr<Label>(new Label(controlData.name, controlData.text,
                                                             controlData.fontColor, controlData.position,
                                                             controlData.margins, controlData.textSize,
                                                             controlData.hasBackground, controlData.isVisible,
                                                             controlData.isUsingPercentage, 
                                                             newHoveredProps,
                                                             controlData.percentagedPosition));
                        label->Init(fontsDir + defaultFont, 
									controlData.leftBackgroundImage,
									controlData.rightBackgroundImage,
									controlData.middleBackgroundImage,
                                    windowWidth, windowHeight);

                        newLayout->AddControl(label);
                    }
                    else if(control->second["control-type"].as<std::string>() == "button")
                    {
                        controlData.name = control->second["name"].as<std::string>();
                        controlData.text = control->second["text"].as<std::string>();
                        controlData.hasBackground = control->second["has-background"].as<bool>();
                        controlData.isVisible = control->second["is-visible"].as<bool>();
                        controlData.isUsingPercentage = control->second["is-using-percentage"].as<bool>();
                        controlData.fontColor = glm::vec4(control->second["font-color"][0].as<float>(),
                                                          control->second["font-color"][1].as<float>(),
                                                          control->second["font-color"][2].as<float>(),
                                                          control->second["font-color"][3].as<float>());
                        controlData.textSize = control->second["text-size"].as<int>();
                        controlData.leftBackgroundImage = "";
                        controlData.rightBackgroundImage = "";
                        controlData.middleBackgroundImage = "";
                        if(controlData.hasBackground)
                        {
                            controlData.margins = glm::vec4(control->second["margins"][0].as<float>(),
                                                            control->second["margins"][1].as<float>(),
                                                            control->second["margins"][2].as<float>(),
                                                            control->second["margins"][3].as<float>());

                            controlData.leftBackgroundImage = texturesDir;
                            controlData.leftBackgroundImage += control->second["bckg-left"].as<std::string>();
							controlData.rightBackgroundImage = texturesDir;
							controlData.rightBackgroundImage += control->second["bckg-right"].as<std::string>();
							controlData.middleBackgroundImage = texturesDir;
							controlData.middleBackgroundImage += control->second["bckg-middle"].as<std::string>();
                        }
                        controlData.percentagedPosition = glm::vec2();
                        controlData.position = glm::vec2();
                        if(controlData.isUsingPercentage)
                        {
                            controlData.percentagedPosition = 
                                glm::vec2(control->second["percentaged-pos"][0].as<float>(),
                                          control->second["percentaged-pos"][1].as<float>());
                        }
                        else
                        {
                            controlData.position = glm::vec2(control->second["position"][0].as<float>(),
                                                             control->second["position"][1].as<float>());
                        }

                        HoveredProperties newHoveredProps;
                        newHoveredProps.backgroundLeftImage = controlData.onHoverLeftBackgroundImage;
						newHoveredProps.backgroundRightImage = controlData.onHoverRightBackgroundImage;
						newHoveredProps.backgroundMiddleImage = controlData.onHoverMiddleBackgroundImage;
                        newHoveredProps.text = controlData.onHoverText;
                        newHoveredProps.textColor = controlData.onHoverFontColor;
                        newHoveredProps.font = controlData.onHoverFont;

                        std::shared_ptr<Button> button = 
                            std::shared_ptr<Button>(new Button(controlData.name, controlData.text,
                                                               controlData.fontColor, controlData.position,
                                                               controlData.margins, controlData.textSize,
                                                               controlData.hasBackground, controlData.isVisible,
                                                               controlData.isUsingPercentage, 
                                                               newHoveredProps,
                                                               controlData.percentagedPosition));
                        button->Init(fontsDir + defaultFont, 
									 controlData.leftBackgroundImage,
									 controlData.rightBackgroundImage,
									 controlData.middleBackgroundImage,
                                     windowWidth, windowHeight);

                        newLayout->AddControl(button);
                    }
                    else if(control->second["control-type"].as<std::string>() == "imageBox")
                    {
                        controlData.name = control->second["name"].as<std::string>();
                        controlData.isVisible = control->second["is-visible"].as<bool>();
                        controlData.isUsingPercentage = control->second["is-using-percentage"].as<bool>();
                        controlData.imageBoxHeight = control->second["height"].as<float>();
                        controlData.imageBoxWidth = control->second["width"].as<float>();
                        controlData.middleBackgroundImage = texturesDir;
                        controlData.middleBackgroundImage += control->second["image"].as<std::string>();
                        controlData.percentagedPosition = glm::vec2();
                        controlData.position = glm::vec2();
                        if(controlData.isUsingPercentage)
                        {
                            controlData.percentagedPosition = 
                                glm::vec2(control->second["percentaged-pos"][0].as<float>(),
                                          control->second["percentaged-pos"][1].as<float>());
                        }
                        else
                        {
                            controlData.position = glm::vec2(control->second["position"][0].as<float>(),
                                                             control->second["position"][1].as<float>());
                        }

                        HoveredProperties newHoveredProps;
                        newHoveredProps.backgroundMiddleImage = controlData.onHoverMiddleBackgroundImage;
                        newHoveredProps.text = controlData.onHoverText;
                        newHoveredProps.textColor = controlData.onHoverFontColor;
                        newHoveredProps.font = controlData.onHoverFont;

                        std::shared_ptr<ImageBox> imageBox = 
                            std::shared_ptr<ImageBox>(new ImageBox(controlData.imageBoxWidth, controlData.imageBoxHeight,
                                                                   controlData.name, controlData.text,
                                                                   controlData.fontColor, controlData.position,
                                                                   controlData.margins, controlData.textSize,
                                                                   controlData.hasBackground, controlData.isVisible,
                                                                   controlData.isUsingPercentage, 
                                                                   newHoveredProps,
                                                                   controlData.percentagedPosition));
                        imageBox->Init(controlData.middleBackgroundImage, windowWidth, windowHeight);

                        newLayout->AddControl(imageBox);
                    }
                    else if(control->second["control-type"].as<std::string>() == "textBox")
                    {
                        controlData.name = control->second["name"].as<std::string>();
                        controlData.text = "";
                        controlData.hasBackground = control->second["has-background"].as<bool>();
                        controlData.isVisible = control->second["is-visible"].as<bool>();
                        controlData.isUsingPercentage = control->second["is-using-percentage"].as<bool>();
                        controlData.fontColor = glm::vec4(control->second["font-color"][0].as<float>(),
                                                          control->second["font-color"][1].as<float>(),
                                                          control->second["font-color"][2].as<float>(),
                                                          control->second["font-color"][3].as<float>());
                        controlData.textSize = control->second["text-size"].as<int>();
                        controlData.textBoxWidth = control->second["width"].as<float>();
                        controlData.leftBackgroundImage = "";
                        controlData.rightBackgroundImage = "";
                        controlData.middleBackgroundImage = "";
                        if(controlData.hasBackground)
                        {
                            controlData.margins = glm::vec4(control->second["margins"][0].as<float>(),
                                                            control->second["margins"][1].as<float>(),
                                                            control->second["margins"][2].as<float>(),
                                                            control->second["margins"][3].as<float>());
							
                            controlData.leftBackgroundImage = texturesDir;
                            controlData.leftBackgroundImage += control->second["bckg-left"].as<std::string>();
							controlData.rightBackgroundImage = texturesDir;
							controlData.rightBackgroundImage += control->second["bckg-right"].as<std::string>();
							controlData.middleBackgroundImage = texturesDir;
							controlData.middleBackgroundImage += control->second["bckg-middle"].as<std::string>();
                        }
                        controlData.percentagedPosition = glm::vec2();
                        controlData.position = glm::vec2();
                        if(controlData.isUsingPercentage)
                        {
                            controlData.percentagedPosition = 
                                glm::vec2(control->second["percentaged-pos"][0].as<float>(),
                                          control->second["percentaged-pos"][1].as<float>());
                        }
                        else
                        {
                            controlData.position = glm::vec2(control->second["position"][0].as<float>(),
                                                             control->second["position"][1].as<float>());
                        }

                        HoveredProperties newHoveredProps;
                        newHoveredProps.backgroundLeftImage = controlData.onHoverLeftBackgroundImage;
						newHoveredProps.backgroundRightImage = controlData.onHoverRightBackgroundImage;
						newHoveredProps.backgroundMiddleImage = controlData.onHoverMiddleBackgroundImage;
                        newHoveredProps.text = controlData.onHoverText;
                        newHoveredProps.textColor = controlData.onHoverFontColor;
                        newHoveredProps.font = controlData.onHoverFont;

                        std::shared_ptr<TextBox> textBox = 
                            std::shared_ptr<TextBox>(new TextBox(controlData.textBoxWidth,
                                                                 controlData.name, controlData.text,
                                                                 controlData.fontColor, controlData.position,
                                                                 controlData.margins, controlData.textSize,
                                                                 controlData.hasBackground, controlData.isVisible,
                                                                 controlData.isUsingPercentage, 
                                                                 newHoveredProps,
                                                                 controlData.percentagedPosition));
                        textBox->Init(fontsDir + defaultFont, 
									  controlData.leftBackgroundImage,
									  controlData.rightBackgroundImage,
									  controlData.middleBackgroundImage,
                                      windowWidth, windowHeight);

                        newLayout->AddControl(textBox);
                    }
                }
            }
        }
    }

	return layouts;
}


/////////////////////////
///  TweakVar Loader  ///
/////////////////////////
const std::vector<std::pair<std::string, TweakVarData>> TweakVarAssetObject::GetAllLoadedVars()
{
	return loadedTweaks;
}


std::pair<std::string, TweakVarData> TweakVarLoader::ToInt(const std::string &command, int value, int enumIndex)
{
	TweakVarData tweakData;
    tweakData.currentType = TweakVarData::TYPE_TWEAK_INT;
    tweakData.varInt = value;
    if(enumIndex != -999)
    {
        tweakData.itemIndex = enumIndex;
    }
	return std::pair<std::string, TweakVarData>(command, tweakData);
}

std::pair<std::string, TweakVarData> TweakVarLoader::ToFloat(const std::string &command, float value, int enumIndex)
{
	TweakVarData tweakData;
    tweakData.currentType = TweakVarData::TYPE_TWEAK_FLOAT;
    tweakData.varFloat = value;
    if(enumIndex != -999)
    {
        tweakData.itemIndex = enumIndex;
    }
	return std::pair<std::string, TweakVarData>(command, tweakData);
}

std::pair<std::string, TweakVarData> TweakVarLoader::ToString(const std::string &command, const std::string &value, int enumIndex)
{
	TweakVarData tweakData;
    tweakData.currentType = TweakVarData::TYPE_TWEAK_STRING;
    std::strcpy(tweakData.varString, value.c_str());
    if(enumIndex != -999)
    {
        tweakData.itemIndex = enumIndex;
    }
	return std::pair<std::string, TweakVarData>(command, tweakData);
}

TweakVarAssetObject TweakVarLoader::Load(const std::string &type, const std::string &name)
{
	YAML::Node tweaks = YAML::LoadFile("../data/" + type + "/" + name);
    
	std::vector<std::pair<std::string, TweakVarData>> loadedTweaks;
    for(YAML::Node::const_iterator tweak = tweaks.begin();
        tweak != tweaks.end(); ++tweak)
    {
        TweakVarData tweakData;
        std::string command = tweak->first.as<std::string>();

        if(command == "currentEnemyCount")
        {
            loadedTweaks.push_back(ToInt(command, tweak->second.as<int>()));
        }
        else if(command == "maxEnemyCount")
        {
            loadedTweaks.push_back(ToInt(command, tweak->second.as<int>()));
        }
        else if(command == "initialSpawnTime")
        {
            loadedTweaks.push_back(ToFloat(command, tweak->second.as<float>()));
        }
        else if(command == "endSpawnTime")
        {
            loadedTweaks.push_back(ToFloat(command, tweak->second.as<float>()));
        }
        else if(command == "timeDecrement")
        {
            loadedTweaks.push_back(ToFloat(command, tweak->second.as<float>()));
        }
        else if(command == "enemyDestructionRadius")
        {
            loadedTweaks.push_back(ToFloat(command, tweak->second.as<float>()));
        }
        else if(command == "resourceCount")
        {
            loadedTweaks.push_back(ToInt(command, tweak->second.as<int>()));
        }
        else if(command == "resourceGainTime")
        {
            for(YAML::Node::const_iterator satType = tweak->second.begin();
                satType != tweak->second.end(); ++satType)
            {
                if(satType->first.as<std::string>() == "sat-all") 
                {
                    loadedTweaks.push_back(ToFloat(command, satType->second.as<float>(), 4));
                }
                else if(satType->first.as<std::string>() == "sat-fire") 
                {
                    loadedTweaks.push_back(ToFloat(command, satType->second.as<float>(), 0));
                }
                else if(satType->first.as<std::string>() == "sat-water")
                {
                    loadedTweaks.push_back(ToFloat(command, satType->second.as<float>(), 1));
                }
                else if(satType->first.as<std::string>() == "sat-air")
                {
                    loadedTweaks.push_back(ToFloat(command, satType->second.as<float>(), 2));
                }
                else if(satType->first.as<std::string>() == "sat-earth")
                {
                    loadedTweaks.push_back(ToFloat(command, satType->second.as<float>(), 3));
                }
            }
        }
        else if(command == "resourceGainPerTime")
        {
            for(YAML::Node::const_iterator satType = tweak->second.begin();
                satType != tweak->second.end(); ++satType)
            {
                if(satType->first.as<std::string>() == "sat-all") 
                {
                    loadedTweaks.push_back(ToInt(command, satType->second.as<int>(), 4));
                }
                else if(satType->first.as<std::string>() == "sat-fire") 
                {
                    loadedTweaks.push_back(ToInt(command, satType->second.as<int>(), 0));
                }
                else if(satType->first.as<std::string>() == "sat-water")
                {
                    loadedTweaks.push_back(ToInt(command, satType->second.as<int>(), 1));
                }
                else if(satType->first.as<std::string>() == "sat-air")
                {
                    loadedTweaks.push_back(ToInt(command, satType->second.as<int>(), 2));
                }
                else if(satType->first.as<std::string>() == "sat-earth")
                {
                    loadedTweaks.push_back(ToInt(command, satType->second.as<int>(), 3));
                }
            }
        }
        else if(command == "satConstructionCost")
        {
            loadedTweaks.push_back(ToInt(command, tweak->second.as<int>()));
        }
        else if(command == "satHealth")
        {
            for(YAML::Node::const_iterator satType = tweak->second.begin();
                satType != tweak->second.end(); ++satType)
            {
                if(satType->first.as<std::string>() == "sat-all") 
                {
                    loadedTweaks.push_back(ToInt(command, satType->second.as<int>(), 4));
                }
                else if(satType->first.as<std::string>() == "sat-fire") 
                {
                    loadedTweaks.push_back(ToInt(command, satType->second.as<int>(), 0));
                }
                else if(satType->first.as<std::string>() == "sat-water")
                {
                    loadedTweaks.push_back(ToInt(command, satType->second.as<int>(), 1));
                }
                else if(satType->first.as<std::string>() == "sat-air")
                {
                    loadedTweaks.push_back(ToInt(command, satType->second.as<int>(), 2));
                }
                else if(satType->first.as<std::string>() == "sat-earth")
                {
                    loadedTweaks.push_back(ToInt(command, satType->second.as<int>(), 3));
                }
            }
        }
        else if(command == "health")
        {
            loadedTweaks.push_back(ToInt(command, tweak->second.as<int>()));
        }
        else if(command == "enemyDamage")
        {
            for(YAML::Node::const_iterator enemyType = tweak->second.begin();
                enemyType != tweak->second.end(); ++enemyType)
            {
                if(enemyType->first.as<std::string>() == "enemy-all")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 5));
                }
                else if(enemyType->first.as<std::string>() == "enemy-swarm")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 0));
                }
                else if(enemyType->first.as<std::string>() == "enemy-spaceship")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 1));
                }
                else if(enemyType->first.as<std::string>() == "enemy-mothership")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 2));
                }
                else if(enemyType->first.as<std::string>() == "enemy-fast-suicide-bomber")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 3));
                }
                else if(enemyType->first.as<std::string>() == "enemy-asteroid")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 4));
                }
            }
        }
        else if(command == "enemyChargeSpeed")
        {
            loadedTweaks.push_back(ToFloat(command, tweak->second.as<float>()));
        }
        else if(command == "enemySpawnInnerRad")
        {
            for(YAML::Node::const_iterator enemyType = tweak->second.begin();
                enemyType != tweak->second.end(); ++enemyType)
            {
                if(enemyType->first.as<std::string>() == "enemy-all")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 5));
                }
                else if(enemyType->first.as<std::string>() == "enemy-swarm")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 0));
                }
                else if(enemyType->first.as<std::string>() == "enemy-spaceship")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 1));
                }
                else if(enemyType->first.as<std::string>() == "enemy-mothership")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 2));
                }
                else if(enemyType->first.as<std::string>() == "enemy-fast-suicide-bomber")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 3));
                }
                else if(enemyType->first.as<std::string>() == "enemy-asteroid")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 4));
                }
            }
        }
        else if(command == "enemySpawnOuterRad")
        {
            for(YAML::Node::const_iterator enemyType = tweak->second.begin();
                enemyType != tweak->second.end(); ++enemyType)
            {
                if(enemyType->first.as<std::string>() == "enemy-all")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 5));
                }
                else if(enemyType->first.as<std::string>() == "enemy-swarm")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 0));
                }
                else if(enemyType->first.as<std::string>() == "enemy-spaceship")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 1));
                }
                else if(enemyType->first.as<std::string>() == "enemy-mothership")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 2));
                }
                else if(enemyType->first.as<std::string>() == "enemy-fast-suicide-bomber")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 3));
                }
                else if(enemyType->first.as<std::string>() == "enemy-asteroid")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 4));
                }
            }
        }
        else if(command == "enemySpeed")
        {
            for(YAML::Node::const_iterator enemyType = tweak->second.begin();
                enemyType != tweak->second.end(); ++enemyType)
            {
                if(enemyType->first.as<std::string>() == "enemy-all")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 5));
                }
                else if(enemyType->first.as<std::string>() == "enemy-swarm")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 0));
                }
                else if(enemyType->first.as<std::string>() == "enemy-spaceship")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 1));
                }
                else if(enemyType->first.as<std::string>() == "enemy-mothership")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 2));
                }
                else if(enemyType->first.as<std::string>() == "enemy-fast-suicide-bomber")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 3));
                }
                else if(enemyType->first.as<std::string>() == "enemy-asteroid")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 4));
                }
            }
        }
        else if(command == "enemyLOS")
        {
            for(YAML::Node::const_iterator enemyType = tweak->second.begin();
                enemyType != tweak->second.end(); ++enemyType)
            {
                if(enemyType->first.as<std::string>() == "enemy-all")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 5));
                }
                else if(enemyType->first.as<std::string>() == "enemy-swarm")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 0));
                }
                else if(enemyType->first.as<std::string>() == "enemy-spaceship")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 1));
                }
                else if(enemyType->first.as<std::string>() == "enemy-mothership")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 2));
                }
                else if(enemyType->first.as<std::string>() == "enemy-fast-suicide-bomber")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 3));
                }
                else if(enemyType->first.as<std::string>() == "enemy-asteroid")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 4));
                }
            }
        }
        else if(command == "enemyHealth")
        {
            for(YAML::Node::const_iterator enemyType = tweak->second.begin();
                enemyType != tweak->second.end(); ++enemyType)
            {
                if(enemyType->first.as<std::string>() == "enemy-all")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 5));
                }
                else if(enemyType->first.as<std::string>() == "enemy-swarm")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 0));
                }
                else if(enemyType->first.as<std::string>() == "enemy-spaceship")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 1));
                }
                else if(enemyType->first.as<std::string>() == "enemy-mothership")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 2));
                }
                else if(enemyType->first.as<std::string>() == "enemy-fast-suicide-bomber")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 3));
                }
                else if(enemyType->first.as<std::string>() == "enemy-asteroid")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 4));
                }
            }
        }
        else if(command == "enemyResource")
        {
            for(YAML::Node::const_iterator enemyType = tweak->second.begin();
                enemyType != tweak->second.end(); ++enemyType)
            {
                if(enemyType->first.as<std::string>() == "enemy-all")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 5));
                }
                else if(enemyType->first.as<std::string>() == "enemy-swarm")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 0));
                }
                else if(enemyType->first.as<std::string>() == "enemy-spaceship")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 1));
                }
                else if(enemyType->first.as<std::string>() == "enemy-mothership")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 2));
                }
                else if(enemyType->first.as<std::string>() == "enemy-fast-suicide-bomber")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 3));
                }
                else if(enemyType->first.as<std::string>() == "enemy-asteroid")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 4));
                }
            }
        }
        else if(command == "swarmersCount")
        {
            loadedTweaks.push_back(ToInt(command, tweak->second.as<int>()));
        }
        else if(command == "swarmersAttackTime")
        {
            loadedTweaks.push_back(ToFloat(command, tweak->second.as<float>()));
        }
        else if(command == "enemyProjectileSpeed")
        {
            loadedTweaks.push_back(ToFloat(command, tweak->second.as<float>()));
        }
        else if(command == "deployUnitsCount")
        {
            loadedTweaks.push_back(ToInt(command, tweak->second.as<int>()));
        }
        else if(command == "deployUnitsLife")
        {
            loadedTweaks.push_back(ToInt(command, tweak->second.as<int>()));
        }
        else if(command == "deployUnitsResourceGivenOnKill")
        {
            loadedTweaks.push_back(ToInt(command, tweak->second.as<int>()));
        }
        else if(command == "deployUnitsSpeed")
        {
            loadedTweaks.push_back(ToFloat(command, tweak->second.as<float>()));
        }
        else if(command == "deployUnitsLOS")
        {
            loadedTweaks.push_back(ToFloat(command, tweak->second.as<float>()));
        }
        else if(command == "deployUnitsProjSpeed")
        {
            loadedTweaks.push_back(ToFloat(command, tweak->second.as<float>()));
        }
        else if(command == "skillDamage")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-all")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 7));
                }
            }
        }
        else if(command == "skillRange")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-all")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 7));
                }
            }
        }
        else if(command == "skillApplyCost")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-all")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 7));
                }
            }
        }
        else if(command == "skillScaleRate")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-all")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 7));
                }
            }
        }
        else if(command == "skillDamageApplyTime")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-all")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 7));
                }
            }
        }
        else if(command == "skillDuration")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-all")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 7));
                }
            }
        }
        else if(command == "skillDefensePoints")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-all")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 7));
                }
            }
        }
        else if(command == "skillStunTime")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-all")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 7));
                }
            }
        }
        else if(command == "skillUpgradeBoxIndex")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-all")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 7));
                }
            }
        }
        else if(command == "skillForWhichSatellite")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-all")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 7));
                }
            }
        }
        else if(command == "skillUpgradedTexture")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-all")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 7));
                }
            }
        }
        else if(command == "skillToolTipText")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-all")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 7));
                }
            }
        }
        else if(command == "skillResearchCost")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-all")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 7));
                }
            }
        }
        else if(command == "skillHintText")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 7));
                }
                else if(skillType->first.as<std::string>() == "skill-fire-sat")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-earth-sat")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 9));
                }
                else if(skillType->first.as<std::string>() == "skill-water-sat")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 10));
                }
                else if(skillType->first.as<std::string>() == "skill-air-sat")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 11));
                }
            }
        }
        else if(command == "skillHintImage")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 7));
                }
                else if(skillType->first.as<std::string>() == "skill-fire-sat")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-earth-sat")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 9));
                }
                else if(skillType->first.as<std::string>() == "skill-water-sat")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 10));
                }
                else if(skillType->first.as<std::string>() == "skill-air-sat")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 11));
                }
            }
        }
    }

	return TweakVarAssetObject(loadedTweaks);
}